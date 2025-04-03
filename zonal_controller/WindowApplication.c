/**
 * @file WindowApplication.c
 * 
 * @brief Implementation of a window zonal controller based on Connext Micro 2.4.14 for an example system.
 * 
 * The example system is expected to contain a total of four windows, managed by a total of two WindowApplication.
 * 
 * The application listens for commands, either remotely (DDS) or local (stdin).
 * The application processes the commands to change the window position, e.g. to fully open the window. 
 * 
 * @define NUM_WINDOWS Number of windows in the application.
 * @define WINDOW_ID_STR_LEN Length of the window ID string.
 * @define WINDOW_POSITION_START Initial position of the windows.
 * @define WINDOW_POSITION_OPEN Position value representing an open window.
 * @define WINDOW_POSITION_CLOSED Position value representing a closed window.
 * @define WINDOW_SPEED_MS Time in milliseconds to wait between window position updates.
 * 
 * @typedef WindowState_t
 * @brief Structure to represent the state of a window.
 * 
 * @struct WindowState
 * @brief Represents the state of a window including its ID, current position, and target position.
 * 
 * @var WindowState::id
 * ID of the window.
 * 
 * @var WindowState::position
 * Current position of the window.
 * 
 * @var WindowState::target
 * Target position of the window.
 * 
 * For processing local commands, the below configurations are relevant:
 * @define MAX_INPUT_SIZE Maximum size of the input buffer for local commands via stdin.
 * @define NUM_COMMANDS Number of commands that can be handled by the application.
 * @define LOCAL_COMMANDS_STR Array of strings for the valid local commands.
 * @define LOCAL_COMMANDS_STR_LEN Array of lengths of the valid local commands.
 * @define LOCAL_COMMANDS_TARGETS Array of target positions for each of the valid local commands.
 * @define LOCAL_COMMANDS_INDEX_SET Index of the "SET" command in the local commands array.
 *
 * @enum return_value_t Return values of the static functions. This is also used to handle the
 * command to the exit the application.
 * 
 **/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef RTI_WIN32
    #include <windows.h>
    #include <conio.h>
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/select.h>
    #include <strings.h>
#endif
#include <fcntl.h>
#include <ctype.h>

#include "rti_me_c.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "Window.h"
#include "WindowSupport.h"
#include "WindowPlugin.h"

#include "WindowApplicationCommon.h"



/* --------------------------------------------
 * Configurations */


// General configurations
#define NUM_WINDOWS 2
#define WINDOW_ID_STR_LEN ID_STR_LEN
#define WINDOW_POSITION_START 50
#define WINDOW_POSITION_OPEN 0
#define WINDOW_POSITION_CLOSED 100
#define WINDOW_SPEED_MS 50
typedef struct WindowState
{
    char * id;
    int position;
    int target;
} WindowState_t;


// Configurations for handling of local commands via stdin
#define MAX_INPUT_SIZE 11
#define NUM_COMMANDS 4
#define LOCAL_COMMANDS_STR {"OPEN", "CLOSE", "SET", "EXIT"}
#define LOCAL_COMMANDS_STR_LEN {5, 6, 4, 5}
#define LOCAL_COMMANDS_TARGETS {WINDOW_POSITION_OPEN, WINDOW_POSITION_CLOSED}
#define LOCAL_COMMANDS_INDEX_SET 2
#define LOCAL_COMMANDS_INDEX_EXIT 3


// Return values of the static functions
typedef enum
{
    RETVAL_ERROR = -1,
    RETVAL_OK = 0,
    RETVAL_EXIT = 2
} return_value_t;



/* --------------------------------------------
 * Connext Micro specific functions */


/**
 * @brief Creates a DDS Publisher for the given DomainParticipant.
 *
 * This function initializes and returns a DDS Publisher in the 
 * provided DomainParticipant, with the intention to use one 
 * publisher for all writers in the application if possible, so as
 * to optimise resource usage.
 *
 * @param participant Pointer to the DDS_DomainParticipant for which the 
 *                    Publisher is to be created.
 * @return A pointer to the created DDS_Publisher, or NULL if the creation 
 *         fails.
 */
RTI_PRIVATE DDS_Publisher *
Application_create_publisher(DDS_DomainParticipant *participant)
{
    DDS_Publisher *publisher;

    publisher = DDS_DomainParticipant_create_publisher(
                    participant,
                    &DDS_PUBLISHER_QOS_DEFAULT,
                    NULL,
                    DDS_STATUS_MASK_NONE);
    if (publisher == NULL)
    {
        printf("publisher == NULL\n");
    }

    return publisher;
}


/**
 * @brief Creates a DDS DataWriter for the given Publisher and Topic.
 *
 * This function initializes and returns a DDS DataWriter in the 
 * provided Publisher for the specified Topic.
 *
 * @param publisher Pointer to the DDS_Publisher for which the 
 *                  DataWriter is to be created.
 * @param topic     Pointer to the DDS_Topic for which the DataWriter 
 *                  is to be created.
 * @param dw_qos    Pointer to the DDS_DataWriterQos structure to be used 
 *                  for creating the DataWriter.
 * @return A pointer to the created DDS_DataWriter, or NULL if the creation 
 *         fails.
 */
RTI_PRIVATE DDS_DataWriter *
Application_create_datawriter(
    DDS_Publisher *publisher,
    DDS_Topic *topic,
    struct DDS_DataWriterQos *dw_qos)
{
    DDS_DataWriter *datawriter;

    dw_qos->resource_limits.max_samples_per_instance = 1;
    dw_qos->resource_limits.max_instances = 2;
    dw_qos->resource_limits.max_samples = dw_qos->resource_limits.max_instances *
                                          dw_qos->resource_limits.max_samples_per_instance;
    dw_qos->durability.kind = DDS_TRANSIENT_LOCAL_DURABILITY_QOS;
    dw_qos->history.depth = 1;
    dw_qos->reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
    dw_qos->protocol.rtps_reliable_writer.heartbeat_period.sec = 0;
    dw_qos->protocol.rtps_reliable_writer.heartbeat_period.nanosec = 250000000;

    datawriter = DDS_Publisher_create_datawriter(
                     publisher,
                     topic,
                     dw_qos,
                     NULL,
                     DDS_STATUS_MASK_NONE);

    return datawriter;
}


/**
 * @brief Listener function to handle DDS commands to move the windows.
 *
 * This function is called when new data is available for the WindowCommand
 * DataReader. It processes the received data samples and updates the 
 * listener_data with the latest valid sample.
 *
 * @param listener_data Pointer to user-defined data that is passed to the 
 *                      listener. In this case, it is expected to be a 
 *                      pointer to a WindowCommand structure. This is how
 *                      the received command is shared to the application
 *                      code for further processing.
 * @param reader        Pointer to the DDS_DataReader from which the data 
 *                      is being read.
 */
RTI_PRIVATE void
WindowCommandSubscriber_on_data_available(
    void *listener_data,
    DDS_DataReader * reader)
{
    WindowCommandDataReader *hw_reader = WindowCommandDataReader_narrow(reader);
    DDS_ReturnCode_t retcode;
    struct DDS_SampleInfo *sample_info = NULL;
    WindowCommand *sample = NULL;

    struct DDS_SampleInfoSeq info_seq = 
    DDS_SEQUENCE_INITIALIZER;
    struct WindowCommandSeq sample_seq = 
    DDS_SEQUENCE_INITIALIZER;

    DDS_Long i;

    retcode = WindowCommandDataReader_take(
        hw_reader, 
        &sample_seq,
        &info_seq,
        DDS_LENGTH_UNLIMITED,
        DDS_ANY_SAMPLE_STATE,
        DDS_ANY_VIEW_STATE,
        DDS_ANY_INSTANCE_STATE);

    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to take data, retcode(%d)\n", retcode);
        goto done;
    }
    
    for (i = 0; i < WindowCommandSeq_get_length(&sample_seq); ++i)
    {
        sample_info = DDS_SampleInfoSeq_get_reference(&info_seq, i);

        if (sample_info->valid_data)
        {
            sample = WindowCommandSeq_get_reference(&sample_seq, i);

            WindowCommand * remote_command = (WindowCommand *) listener_data;
            remote_command->id = sample->id;
            remote_command->position = sample->position;
        }
        else
        {
            printf("\nSample received with non-valid data\n");
        }
    }

    WindowCommandDataReader_return_loan(hw_reader, &sample_seq, &info_seq);

    done:
#ifndef RTI_CERT
    WindowCommandSeq_finalize(&sample_seq);
    DDS_SampleInfoSeq_finalize(&info_seq);
#else
    return;
#endif
}


/**
 * @brief Creates a DDS Subscriber for the given DomainParticipant.
 *
 * This function initializes and returns a DDS Subscriber in the 
 * provided DomainParticipant, with the intention to use one 
 * subscriber for all readers in the application if possible, so as
 * to optimise resource usage.
 *
 * @param participant Pointer to the DDS_DomainParticipant for which the 
 *                    Subscriber is to be created.
 * @return A pointer to the created DDS_Subscriber, or NULL if the creation 
 *         fails.
 */
RTI_PRIVATE DDS_Subscriber *
Application_create_subscriber(DDS_DomainParticipant *participant)
{
    DDS_Subscriber *subscriber;

    subscriber = DDS_DomainParticipant_create_subscriber(
                    participant,
                    &DDS_SUBSCRIBER_QOS_DEFAULT,
                    NULL,
                    DDS_STATUS_MASK_NONE);
    if (subscriber == NULL)
    {
        printf("subscriber == NULL\n");
    }

    return subscriber;
}


/**
 * @brief Creates a DDS DataReader for the given Subscriber and Topic.
 *
 * This function initializes and returns a DDS DataReader in the 
 * provided Subscriber for the specified Topic.
 *
 * @param subscriber Pointer to the DDS_Subscriber for which the 
 *                   DataReader is to be created.
 * @param topic      Pointer to the DDS_Topic for which the DataReader 
 *                   is to be created.
 * @param dr_qos     Pointer to the DDS_DataReaderQos structure to be used 
 *                   for creating the DataReader.
 * @param listener_data Pointer to user-defined data that is passed to the 
 *                      listener. In this case, it is expected to be a 
 *                      pointer to a WindowCommand structure. This is how
 *                      the received command is shared to the application
 *                      code for further processing.
 * @return A pointer to the created DDS_DataReader, or NULL if the creation 
 *         fails.
 */
RTI_PRIVATE DDS_DataReader *
Application_create_datareader(
    DDS_Subscriber *subscriber,
    DDS_Topic *topic,
    struct DDS_DataReaderQos *dr_qos,
    void * listener_data)
{
    DDS_DataReader *datareader;
    struct DDS_DataReaderListener dr_listener = DDS_DataReaderListener_INITIALIZER;

    dr_qos->resource_limits.max_instances = 4;
    dr_qos->resource_limits.max_samples_per_instance = 1;
    dr_qos->resource_limits.max_samples = dr_qos->resource_limits.max_instances *
                                          dr_qos->resource_limits.max_samples_per_instance;
    /* if there are more remote writers, you need to increase these limits */
    dr_qos->reader_resource_limits.max_remote_writers = 2;
    dr_qos->reader_resource_limits.max_remote_writers_per_instance = 1;
    dr_qos->history.depth = 1;
    dr_qos->durability.kind = DDS_VOLATILE_DURABILITY_QOS;
    dr_qos->reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;

    dr_listener.on_data_available = WindowCommandSubscriber_on_data_available;
    dr_listener.as_listener.listener_data = listener_data;

    datareader = DDS_Subscriber_create_datareader(
        subscriber,
        DDS_Topic_as_topicdescription(topic),
        dr_qos,
        &dr_listener,
        DDS_DATA_AVAILABLE_STATUS);

    return datareader;
}



/* --------------------------------------------
 * Application specific functions */


/**
 * @brief Reads a line from the standard input without blocking.
 *
 * This function attempts to read a line from the standard input into the provided buffer.
 * It does not block if there is no input available.
 *
 * @param buffer A pointer to the buffer where the read line will be stored.
 * @param size The size of the buffer.
 * @return A pointer to the buffer containing the read line, or NULL if no input is available.
 */
RTI_PRIVATE char *
non_blocking_fgets(char *buffer, int size)
{
#ifdef _WIN32
    // Windows implementation using _kbhit() and _getch()
    if (_kbhit()) {
        if (fgets(buffer, size, stdin) != NULL) {
            // Remove trailing newline if present
            int len = (int)strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            return buffer;
        }
    }
    return NULL; // No input available
#else
    // POSIX implementation
    int flags, available;
    fd_set readfds;
    struct timeval timeout;

    // Set stdin to non-blocking mode
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    timeout.tv_sec = 0;   // 0 seconds
    timeout.tv_usec = 0;  // 0 microseconds

    available = select(1, &readfds, NULL, NULL, &timeout);

    if (available > 0) {
        if (fgets(buffer, size, stdin) != NULL) {
            int len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            fcntl(STDIN_FILENO, F_SETFL, flags);
            return buffer;
        }
    }

    fcntl(STDIN_FILENO, F_SETFL, flags);
    return NULL;
#endif
}


/**
 * @brief Publish the given window state, always (check_target==0) or if the target is different from the current position (check_target==1)
 * 
 * @param window the WindowState_t to publish
 * @param check_target 0 to publish regardless of the target and current position, 1 to publish only if the target is different from the current position
 * @param sample the sample instance to use for publishing
 * @param datawriter the datawriter to use for publishing
 */
RTI_PRIVATE void 
publish_window_state(WindowState_t *window, int check_target, WindowUpdate *sample, WindowUpdateDataWriter *datawriter)
{
    DDS_ReturnCode_t retcode;

    if ( check_target == 0 || (window->position != window->target) )
    {
        if (check_target == 1)
        {
            short delta = (window->position > window->target) ? -1 : 1;
            window->position += delta;
        }
        sample->position = window->position;
        sample->id = DDS_String_dup(window->id);

        retcode = WindowUpdateDataWriter_write(
            datawriter,
            sample,
            &DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK)
        {
            printf("Failed to write sample\n");
        }
    }
}


/**
 * @brief Publish the window state for all windows
 *
 * @param windows The array of WindowState_t to publish.
 * @param check_target For each window: 0 to publish regardless of the target and current position, 1 to publish only if the target is different from the current position.
 * @param sample The sample instance to use for publishing.
 * @param datawriter The datawriter to use for publishing.
 */
RTI_PRIVATE void 
publish_window_states(WindowState_t *windows, int check_target, WindowUpdate *sample, WindowUpdateDataWriter *datawriter)
{
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        publish_window_state(&windows[i], check_target, sample, datawriter);
    }
}


/**
 * @brief Process a command (remote or local) by checking for a matching window id and updating the target position.
 * 
 * @param window_id String to match in the windows array.
 * @param target Target position to set for the window in the array.
 * @param windows Array of windows.
 * @return OK if command processed successfully, ERROR or otherwise.
 */
RTI_PRIVATE return_value_t 
process_command(char * window_id, int target, WindowState_t *windows)
{
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        if (strcasecmp(window_id, windows[i].id) == 0)
        {
            windows[i].target = target;
            printf("Processed command %s %d\n", window_id, target);
            return RETVAL_OK;
        }
    }
    return RETVAL_ERROR;
}


/**
 * @brief Checks the local command for the given window state.
 *
 * This function processes the provided command and determines if it is a valid
 * command for the current window state.
 *
 * @param windows The array of windows, which will be updated with the target position if the command is valid.
 * @return OK if command processed sucessfully, ERROR otherwise
 */
RTI_PRIVATE return_value_t 
check_local_command(WindowState_t * windows)
{
    char input[MAX_INPUT_SIZE];
    char *command_local = non_blocking_fgets(input, MAX_INPUT_SIZE);
    char *commands[NUM_COMMANDS] = LOCAL_COMMANDS_STR;
    int commands_targets[NUM_COMMANDS] = LOCAL_COMMANDS_TARGETS;
    int commands_str_len[NUM_COMMANDS] = LOCAL_COMMANDS_STR_LEN;
    return_value_t retval = RETVAL_ERROR;
    
    if (command_local != NULL)
    {
        // Loop over the array of possible local commands
        for (int i=0; i < NUM_COMMANDS; i++)
        {
            // Case insensitive check to compare the stdin command to possible local commands
            if (strncasecmp(command_local, commands[i], commands_str_len[i]-1) == 0)
            {
                // Check for the special case of the "EXIT" command
                if ( i == LOCAL_COMMANDS_INDEX_EXIT )
                {
                    printf("Processing exit of the application\n");
                    return RETVAL_EXIT;
                }
                
                // Extract the window id from the stdin command
                char window_id[WINDOW_ID_STR_LEN + 1];
                strncpy(window_id, command_local + commands_str_len[i], WINDOW_ID_STR_LEN);
                window_id[WINDOW_ID_STR_LEN] = '\0'; // Null-terminate the string
                
                // Set the target position accordingly
                int target = commands_targets[i];
                
                // Check for the special case of setting the specific target position
                if ( i == LOCAL_COMMANDS_INDEX_SET )
                {
                    char *target_str = command_local + commands_str_len[i] + WINDOW_ID_STR_LEN;
                    target = atoi(target_str);
                    target = (target < 0) ? 0 : (target > 100) ? 100 : target;
                }

                // Return with success or failure in processing the command
                retval = process_command(window_id, target, windows);

                break;
            }
        }
    }

    return retval;
}


/**
 * @brief Main loop for window controller application.
 *
 * In this loop, we continuously monitor for remote commands and local commands.
 * Remote commands are inbound via DDS and update the command structure for processing.
 * Local commands come from stdin and are checked and processed directly.
 * 
 * @note Any valid command interrupts and overrides any previous command.
 * 
 * @note Local commands take precedence over remote commands as they are processed afterwards in the same loop.
 *
 * @param windows    Array of windows managed by this application, for updating the target position etc.
 * @param command    Reference to the structure in which remote commands are stored. Used to process these commands.
 * @param sample     Pointer to the sample to use for publishing window position updates.
 * @param datawriter Pointer to the datawriter to use for publishing window position updates.
 */
RTI_PRIVATE void 
main_loop(WindowState_t *windows, WindowCommand *command, WindowUpdate *sample, WindowUpdateDataWriter *datawriter)
{
    return_value_t retval = RETVAL_ERROR;

    // Continuously monitor for commands and update the windows if needed
    while ( retval != RETVAL_EXIT )
    {
        // Check if a remote command was issued for our window_ids
        if (command->id != NULL)
        {
            if ( RETVAL_OK == process_command(command->id, command->position, windows) )
            {
                // Reset the remote command structure after processing
                command->id = "";
            }
        }

        // Check if there is a local command and process it if valid
        retval = check_local_command(windows);
        if ( retval == RETVAL_OK )
        {
            // Reset any remote command that might have come before or during the processing
            command->id = "";
        }
        else if ( retval == RETVAL_EXIT )
        {
            // Early exit if the "EXIT" command was issued
            printf("Exiting the application\n");
            break;
        }

        // Update the position and send DDS update if moving any window
        publish_window_states(windows, 1, sample, datawriter);

        // Sleep to allow the windows to move at the specified speed
        OSAPI_Thread_sleep((RTI_UINT32)WINDOW_SPEED_MS);
    }
}


/**
 * @brief Main function with arguments for the Window Application.
 *
 * It initializes necessary the Connext Micro components and entities
 * and starts the application loop.
 *
 * @param domain_id The DDS domain_id to use.
 * @param udp_intf  The specific UDP interface to use (or NULL for default).
 * @param peer      The specific peer address to use for the participant (or NULL for default).
 * @return An integer representing the exit status of the application.
 */
RTI_PRIVATE int
main_w_args(
    DDS_Long domain_id,
    char *udp_intf,
    char *peer,
    char **window_ids)
{
    DDS_DomainParticipant *participant;
    DDS_Publisher *publisher;
    DDS_Subscriber *subscriber;
    DDS_Topic *topic_window_command;
    DDS_Topic *topic_window_update;
    WindowUpdateDataWriter *datawriter;
    struct DDS_DataWriterQos dw_qos = DDS_DataWriterQos_INITIALIZER;
    DDS_DataReader *datareader;
    struct DDS_DataReaderQos dr_qos = DDS_DataReaderQos_INITIALIZER;
    return_value_t retval = RETVAL_ERROR;
    DDS_ReturnCode_t retcode;
    WindowUpdate *sample = NULL;

    WindowState_t windows[NUM_WINDOWS];
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        windows[i].id = window_ids[i];
        windows[i].position = WINDOW_POSITION_START;
        windows[i].target = WINDOW_POSITION_START;
    }

    sample = WindowUpdateTypeSupport_create_data();
    if (sample == NULL)
    {
        printf("failed WindowUpdateTypeSupport_create_data\n");
        return -1;
    }

    if (Application_initialize_micro(udp_intf) != DDS_BOOLEAN_TRUE)
    {
        printf("Failed to initialize micro\n");
        goto done;
    }

    participant = Application_create_participant(domain_id, peer);
    if (participant == NULL)
    {
        printf("failed participant create\n");
        goto done;
    }

    topic_window_update = Application_register_type_and_create_topic_window_update(participant);
    if (topic_window_update == NULL)
    {
        printf("topic_window_update == NULL\n");
        goto done;
    }

    topic_window_command = Application_register_type_and_create_topic_window_command(participant);
    if (topic_window_update == NULL)
    {
        printf("topic_window_command == NULL\n");
        goto done;
    }

    publisher = Application_create_publisher(participant);
    if (publisher == NULL)
    {
        printf("publisher == NULL\n");
        goto done;
    }

    datawriter = (WindowUpdateDataWriter*) Application_create_datawriter(publisher, topic_window_update, &dw_qos);
    if (datawriter == NULL)
    {
        printf("datawriter == NULL\n");
        goto done;
    }
    datawriter = WindowUpdateDataWriter_narrow((DDS_DataWriter *) datawriter);

    // Structure to hold window commands to process
    WindowCommand command;
    command.id = NULL;
    command.position = WINDOW_POSITION_START;

    subscriber = Application_create_subscriber(participant);
    if (subscriber  == NULL)
    {
        printf("subscriber == NULL\n");
        goto done;
    }

    datareader = Application_create_datareader(subscriber, topic_window_command, &dr_qos, &command);
    if (datareader == NULL)
    {
        printf("datareader == NULL\n");
        goto done;
    }

    // Publish the initial state of the windows (ignore any target position)
    publish_window_states(windows, 0, sample, datawriter);

    // Main loop
    main_loop(windows, &command, sample, datawriter);

    printf("Exiting the application\n");

    retval = RETVAL_OK;

    done:

#ifndef RTI_CERT
    if (participant != NULL)
    {
        Application_delete(participant);
    }
    if (sample != NULL)
    {
        WindowUpdateTypeSupport_delete_data(sample);
    }
    retcode = DDS_DataWriterQos_finalize(&dw_qos);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("Cannot finalize DataWriterQos\n");
        return RETVAL_ERROR;
    }
#endif

    return retval;
}


/**
 * @brief Entry point for the WindowApplication.
 *
 * Refer to WindowApplicationCommon:Application_help for more details on the command line arguments.
 */
int
main(int argc, char **argv)
{
    DDS_Long i = 0;
    DDS_Long domain_id = 0;
    char *peer = NULL;
    char *udp_intf = NULL;
    char * window_ids[WINDOW_ID_STR_LEN] = {"FL", "FR"};

    for (i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-domain"))
        {
            ++i;
            if (i == argc)
            {
                printf("-domain <domain_id>\n");
                return -1;
            }
            domain_id = (DDS_Long)strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-udp_intf"))
        {
            ++i;
            if (i == argc)
            {
                printf("-udp_intf <interface>\n");
                return -1;
            }
            udp_intf = argv[i];
        }
        else if (!strcmp(argv[i], "-peer"))
        {
            ++i;
            if (i == argc)
            {
                printf("-peer <address>\n");
                return -1;
            }
            peer = argv[i];
        }
        else if (!strcmp(argv[i], "-ids"))
        {
            ++i;
            if (i == argc)
            {
                printf("-ids <window_id_1>,<window_id_2>\n");
                return -1;
            }
            char *token = strtok(argv[i], ",");
            int index = 0;
            while (token != NULL && index < 2)
            {
                window_ids[index++] = token;
                token = strtok(NULL, ",");
            }
            if (index != 2)
            {
                printf("Invalid window_ids format. Expected format: \"AB CD\"\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-h"))
        {
            Application_help(argv[0]);
            return 0;
        }
        else
        {
            printf("unknown option: %s\n", argv[i]);
            return -1;
        }
    }    
    
    return main_w_args(domain_id, udp_intf, peer, window_ids);
}