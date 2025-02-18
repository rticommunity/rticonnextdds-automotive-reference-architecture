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
 *
 **/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <ctype.h>

#include "rti_me_c.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "Window.h"
#include "WindowSupport.h"
#include "WindowPlugin.h"

#include "WindowApplicationCommon.h"


// General configurations
#define NUM_WINDOWS 2
#define WINDOW_ID_STR_LEN 2
#define WINDOW_POSITION_START 50
#define WINDOW_POSITION_OPEN 0
#define WINDOW_POSITION_CLOSED 100
typedef struct WindowState
{
    char * id;
    int position;
    int target;
} WindowState_t;

// Configurations for handling of local commands via stdin
#define MAX_INPUT_SIZE 11
#define NUM_COMMANDS 3
#define LOCAL_COMMANDS_STR {"OPEN", "CLOSE", "SET"}
#define LOCAL_COMMANDS_STR_LEN {5, 6, 4}
#define LOCAL_COMMANDS_TARGETS {WINDOW_POSITION_OPEN, WINDOW_POSITION_CLOSED}


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

RTI_PRIVATE DDS_DataWriter *
Application_create_datawriter(
    DDS_Publisher *publisher,
        DDS_Topic *topic,
        struct DDS_DataWriterQos *dw_qos)
{
    DDS_DataWriter *datawriter;

    #ifdef USE_RELIABLE_QOS
    dw_qos->reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
    #else
    dw_qos->reliability.kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
    #endif
    dw_qos->resource_limits.max_samples_per_instance = 1;
    dw_qos->resource_limits.max_instances = 4;
    dw_qos->resource_limits.max_samples = dw_qos->resource_limits.max_instances *
                                          dw_qos->resource_limits.max_samples_per_instance;
    dw_qos->durability.kind = DDS_TRANSIENT_LOCAL_DURABILITY_QOS;
    dw_qos->history.depth = 1;
    dw_qos->protocol.rtps_reliable_writer.heartbeat_period.sec = 0;
    dw_qos->protocol.rtps_reliable_writer.heartbeat_period.nanosec = 250000000;

    datawriter = DDS_Publisher_create_datawriter(
                     publisher,
                     topic,
                     dw_qos,
                     NULL,
                     DDS_STATUS_MASK_NONE);

    done:

    return datawriter;
}


static void
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

    /* Print each valid sample taken */
    for (i = 0; i < WindowCommandSeq_get_length(&sample_seq); ++i)
    {
        sample_info = DDS_SampleInfoSeq_get_reference(&info_seq, i);

        if (sample_info->valid_data)
        {
            sample = WindowCommandSeq_get_reference(&sample_seq, i);

            WindowCommand * remote_command = (WindowCommand *) listener_data;
            remote_command->id = sample->id;
            remote_command->position = sample->position;
            printf("Incoming command: %s %d\n", remote_command->id, remote_command->position);
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

    /* Reliability QoS */
    #ifdef USE_RELIABLE_QOS
    dr_qos->reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
    #else
    dr_qos->reliability.kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
    #endif

    dr_listener.on_data_available = WindowCommandSubscriber_on_data_available;
    dr_listener.as_listener.listener_data = listener_data;

    datareader = DDS_Subscriber_create_datareader(
        subscriber,
        DDS_Topic_as_topicdescription(topic),
        dr_qos,
        &dr_listener,
        DDS_DATA_AVAILABLE_STATUS);

    done:

    return datareader;
}


char *non_blocking_fgets(char *buffer, int size) {
    int flags, available;
    fd_set readfds;
    struct timeval timeout;

    // Set stdin to non-blocking mode
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);


    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    // Set a timeout if you want to avoid indefinite blocking if no input arrives immediately.
    //  If you want it to be truly non-blocking and return immediately even if no input is available,
    //  set timeout.tv_sec and timeout.tv_usec to 0.
    timeout.tv_sec = 0;   // 0 seconds
    timeout.tv_usec = 0;  // 0 microseconds

    available = select(1, &readfds, NULL, NULL, &timeout); // Check if input is available

    if (available > 0) {  // Input is available
        if (fgets(buffer, size, stdin) != NULL) {
            // Remove trailing newline if present (fgets keeps it)
            int len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            
            //Restore the original flags
            fcntl(STDIN_FILENO, F_SETFL, flags);
            return buffer;
        }
    }
    
    //Restore the original flags even if there was no input. Important!
    fcntl(STDIN_FILENO, F_SETFL, flags);
    return NULL; // No input available within the timeout, or error.
}

// Function to convert a string to lowercase
void str_to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}
// Function to convert a string to uppercase
void str_to_upper(char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}


/// @brief Publish the given window state, always (check_target==0) or if the target is different from the current position (check_target==1)
/// @param window the WindowState_t to publish
/// @param check_target 0 to publish regardless of the target and current position, 1 to publish only if the target is different from the current position
/// @param sample the sample instance to use for publishing
/// @param datawriter the datawriter to use for publishing
static void publish_window_state(WindowState_t *window, int check_target, WindowUpdate *sample, WindowUpdateDataWriter *datawriter)
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

/// @brief Publish the window state for all windows
/// @param window the array of WindowState_t to publish
/// @param check_target for each window: 0 to publish regardless of the target and current position, 1 to publish only if the target is different from the current position
/// @param sample the sample instance to use for publishing
/// @param datawriter the datawriter to use for publishing
static void publish_window_states(WindowState_t *windows, int check_target, WindowUpdate *sample, WindowUpdateDataWriter *datawriter)
{
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        publish_window_state(&windows[i], check_target, sample, datawriter);
    }
}


 
/// @brief Process a command (remote or local) by checking for a matching window id and updating the target position
/// @param window_id string to match in the windows array.
/// @param target target position to set for the window in the array.
/// @param windows array of windows.
/// @return 1 if command processed sucessfully, 0 otherwise
static int process_command(char * window_id, int target, WindowState_t *windows)
{
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        if (strcasecmp(window_id, windows[i].id) == 0)
        {
            windows[i].target = target;
            printf("Processed command %s %d\n", window_id, target);
            return 1;
        }
    }
    return 0;
}



static int
main_w_args(
    DDS_Long domain_id,
    char *udp_intf,
    char *peer,
    DDS_Long sleep_time,
    char **window_ids)
{
    DDS_DomainParticipant *participant;
    DDS_Publisher *publisher;
    DDS_Subscriber *subscriber;
    DDS_Topic *topic_window_command;
    DDS_Topic *topic_window_update;
    DDS_DataWriter *_datawriter;
    WindowUpdateDataWriter *datawriter;
    struct DDS_DataWriterQos dw_qos = DDS_DataWriterQos_INITIALIZER;
    DDS_DataReader *datareader;
    struct DDS_DataReaderQos dr_qos = DDS_DataReaderQos_INITIALIZER;
    int ret_value = -1;
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

    _datawriter = Application_create_datawriter(publisher, topic_window_update, &dw_qos);
    if (_datawriter == NULL)
    {
        printf("datawriter == NULL\n");
        goto done;
    }
    datawriter = WindowUpdateDataWriter_narrow(_datawriter);

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

    // Continuously monitor for commands and update the windows if needed
    while (1)
    {
        // Check if a remote command was issued for our window_ids
        if (command.id)
        {
            if ( process_command(command.id, command.position, windows) )
            {
                // Reset the remote command structure after processing
                command.id = "";
            }
        }

        // Check if there is a local command on stdin (non-blocking) and store the target_id
        char input[MAX_INPUT_SIZE];
        char *command_local = non_blocking_fgets(input, MAX_INPUT_SIZE);
        #define NUM_COMMANDS 3
        const char *commands[NUM_COMMANDS] = {"OPEN", "CLOSE", "SET"};
        const int commands_targets[NUM_COMMANDS] = {WINDOW_POSITION_OPEN, WINDOW_POSITION_CLOSED};
        const int commands_len[NUM_COMMANDS] = {5, 6, 4};
        if (command_local != NULL)
        {
            // Loop over the array of possible local commands
            for (int i=0; i<NUM_COMMANDS; i++)
            {
                // Case insensitive check to compare the stdin command to possible local commands
                if (strncasecmp(command_local, commands[i], commands_len[i]-1) == 0)
                {
                    // The stdin command matches one of the valid local commands
                    // Extract the window id from the stdin command
                    char window_id[WINDOW_ID_STR_LEN + 1];
                    strncpy(window_id, command_local + commands_len[i], WINDOW_ID_STR_LEN);
                    window_id[WINDOW_ID_STR_LEN] = '\0'; // Null-terminate the string
                    // Set the target position accordingly
                    int target = commands_targets[i];
                    // Check for the special case of setting the specific target position
                    if (commands[i] == "SET")
                    {
                        char *target_str = command_local + commands_len[i] + WINDOW_ID_STR_LEN;
                        target = atoi(target_str);
                        target = (target < 0) ? 0 : (target > 100) ? 100 : target;
                    }
                    if ( process_command(window_id, target, windows) )
                    {
                        // Reset any remote command that might have come before or during the processing
                        command.id = "";
                    }
                    break;
                }
            }
        }

        // Update the position and send DDS update if moving any window
        publish_window_states(windows, 1, sample, datawriter);

        OSAPI_Thread_sleep((RTI_UINT32)sleep_time);
    }

    ret_value = 0;

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

    #endif
    #ifndef RTI_CERT
    retcode = DDS_DataWriterQos_finalize(&dw_qos);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("Cannot finalize DataWriterQos\n");
        return -1;
    }

    #endif

    return ret_value;
}

int
main(int argc, char **argv)
{
    DDS_Long i = 0;
    DDS_Long domain_id = 0;
    char *peer = NULL;
    char *udp_intf = NULL;
    DDS_Long sleep_time = 50;
    char * window_ids[WINDOW_ID_STR_LEN] = {"AB", "CD"};

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
        else if (!strcmp(argv[i], "-sleep"))
        {
            ++i;
            if (i == argc)
            {
                printf("-sleep_time <sleep_time>\n");
                return -1;
            }
            sleep_time = (DDS_Long)strtol(argv[i], NULL, 0);
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
    printf("Using window_ids: %s , %s\n", window_ids[0], window_ids[1]);
    return main_w_args(domain_id, udp_intf, peer, sleep_time, window_ids);
}
