#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "Window.h"
#include "WindowSupport.h"
#include "WindowPlugin.h"

#include "WindowApplication.h"

static void
WindowUpdatePublisher_on_publication_matched(
    void *listener_data,
    DDS_DataWriter *writer,
    const struct DDS_PublicationMatchedStatus *status)
{
    (void)listener_data;
    (void)writer;

    if (status->current_count_change > 0)
    {
        printf("Matched a subscriber\n");
    }
    else if (status->current_count_change < 0)
    {
        printf("Unmatched a subscriber\n");
    }
}


#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>

#define MAX_INPUT_SIZE 6 // OPEN, CLOSE

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

#include <string.h>
#include <ctype.h>
// Function to convert a string to lowercase
void str_to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}


static void
WindowCommandSubscriber_on_subscription_matched(
    void *listener_data,
    DDS_DataReader *reader,
    const struct DDS_SubscriptionMatchedStatus *status)
{
    (void)listener_data;
    (void)reader;

    if (status->current_count_change > 0)
    {
        printf("Matched a publisher\n");
    }
    else if (status->current_count_change < 0)
    {
        printf("Unmatched a publisher\n");
    }
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
            printf("\nValid sample received\n");

            printf("- id: %s , position %d\n", sample->id, sample->position);
            int sample_id = 0;
            for (int i=0; i < 2; i++)
            {
                sample_id = sample_id | (sample->id[i] << i*8);
            }
            printf("sample_id 0x%x\n", sample_id);
            int * target = (int *) listener_data;
            *target = sample_id << 8 | sample->position;
            printf("Storing incoming target %d\n", *target);
        }
        else
        {
            printf("\nSample received\n\tINVALID DATA\n");
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


static int
publisher_main_w_args(
    DDS_Long domain_id,
    char *udp_intf,
    char *peer,
    DDS_Long sleep_time,
    DDS_Long count, 
    char *window_id)
{
    // Publisher
    DDS_Publisher *publisher;
    DDS_DataWriter *datawriter;
    WindowUpdateDataWriter *hw_datawriter;
    struct DDS_DataWriterQos dw_qos = DDS_DataWriterQos_INITIALIZER;
    DDS_ReturnCode_t retcode;
    WindowUpdate *sample = NULL;
    struct Application *application = NULL;
    struct DDS_DataWriterListener dw_listener = DDS_DataWriterListener_INITIALIZER;
    int ret_value = -1;

    sample = WindowUpdateTypeSupport_create_data();
    if (sample == NULL)
    {
        printf("failed WindowUpdateTypeSupport_create_data\n");
        return -1;
    }

    application = Application_create(
        "publisher",
        "subscriber",
        domain_id,
        udp_intf,
        peer,
        sleep_time,
        count);

    if (application == NULL)
    {
        printf("failed Application create\n");
        goto done;
    }

    publisher = DDS_DomainParticipant_create_publisher(
        application->participant,
        &DDS_PUBLISHER_QOS_DEFAULT,
        NULL,
        DDS_STATUS_MASK_NONE);
    if (publisher == NULL)
    {
        printf("publisher == NULL\n");
        goto done;
    }

    #ifdef USE_RELIABLE_QOS
    dw_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
    #else
    dw_qos.reliability.kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
    #endif
    dw_qos.resource_limits.max_samples_per_instance = 32;
    dw_qos.resource_limits.max_instances = 2;
    dw_qos.resource_limits.max_samples = dw_qos.resource_limits.max_instances *
    dw_qos.resource_limits.max_samples_per_instance;
    dw_qos.history.depth = 32;
    dw_qos.protocol.rtps_reliable_writer.heartbeat_period.sec = 0;
    dw_qos.protocol.rtps_reliable_writer.heartbeat_period.nanosec = 250000000;

    dw_listener.on_publication_matched = WindowUpdatePublisher_on_publication_matched;

    datawriter = DDS_Publisher_create_datawriter(
        publisher,
        application->topic_window_update,
        &dw_qos,
        &dw_listener,
        DDS_PUBLICATION_MATCHED_STATUS);

    if (datawriter == NULL)
    {
        printf("datawriter == NULL\n");
        goto done;
    }

    hw_datawriter = WindowUpdateDataWriter_narrow(datawriter);

    #ifdef RTI_CERT
    #ifdef RTI_VXWORKS
    /** End initialization, disable further dynamic memory allocation ***/
    memAllocDisable();
    #endif
    #endif


    // Subscriber
    DDS_Subscriber *subscriber;
    DDS_DataReader *datareader;
    struct DDS_DataReaderQos dr_qos = DDS_DataReaderQos_INITIALIZER;

    struct DDS_DataReaderListener dr_listener = 
    DDS_DataReaderListener_INITIALIZER;


    subscriber = DDS_DomainParticipant_create_subscriber(
        application->participant,
        &DDS_SUBSCRIBER_QOS_DEFAULT,
        NULL,
        DDS_STATUS_MASK_NONE);
    if (subscriber == NULL)
    {
        printf("subscriber == NULL\n");
        goto done;
    }

    /* Publisher sends samples with id = 0 or id = 1, so 2 instances maximum.
    * But in case filtering is done, all samples with 'id = 0' are
    * filtered so only one instance is needed.
    */
    #ifdef USE_SAMPLE_FILTER
    dr_qos.resource_limits.max_instances = 1;
    #else
    dr_qos.resource_limits.max_instances = 2;
    #endif

    dr_qos.resource_limits.max_samples_per_instance = 32;
    dr_qos.resource_limits.max_samples = dr_qos.resource_limits.max_instances *
    dr_qos.resource_limits.max_samples_per_instance;
    /* if there are more remote writers, you need to increase these limits */
    dr_qos.reader_resource_limits.max_remote_writers = 10;
    dr_qos.reader_resource_limits.max_remote_writers_per_instance = 10;
    dr_qos.history.depth = 32;

    /* Reliability QoS */
    #ifdef USE_RELIABLE_QOS
    dr_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
    #else
    dr_qos.reliability.kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
    #endif

    #ifdef USE_SAMPLE_FILTER
    /* choose one callback to enable */
    #ifdef FILTER_ON_DESERIALIZE
    dr_listener.on_before_sample_deserialize =
    WindowCommandSubscriber_on_before_sample_deserialize;
    #else
    dr_listener.on_before_sample_commit =
    WindowCommandSubscriber_on_before_sample_commit;
    #endif  /* FILTER_ON_DESERIALIZE */
    #endif  /* USE_SAMPLE_FILTER */

    dr_listener.on_data_available = WindowCommandSubscriber_on_data_available;
    dr_listener.on_subscription_matched =
    WindowCommandSubscriber_on_subscription_matched;

    #define POSITION_START 50
    sample->position = POSITION_START;
    int target = POSITION_START;
    int command_remote = 0;
    
    dr_listener.as_listener.listener_data = &command_remote;

    datareader = DDS_Subscriber_create_datareader(
        subscriber,
        DDS_Topic_as_topicdescription(application->topic_window_command),
        &dr_qos,
        &dr_listener,
        DDS_DATA_AVAILABLE_STATUS | DDS_SUBSCRIPTION_MATCHED_STATUS);

    if (datareader == NULL)
    {
        printf("datareader == NULL\n");
        goto done;
    }

    while (1)
    {
        // Check if a command was issued for our window_id
        int id = (command_remote & (0xFFFF00)) >> 8;
        if (id != 0)
        {
            int id_check = id;
            for (int i=0; i<2; i++)
            {
                if ( window_id[i] != (id_check & 0xFF) )
                {
                    id = 0;
                    break;
                }
                else
                {
                    id_check = id_check >> 8;
                }
            }
            if ( id != 0 )
            {
                target = command_remote & 0xFF;
                printf("Received valid remote command - target now %d\n", target);
                command_remote = 0;
            }
        }

        // Check if there is a local command on stdin (non-blocking)
        char input[MAX_INPUT_SIZE];
        char *command_local = non_blocking_fgets(input, MAX_INPUT_SIZE);
        if (command_local != NULL) {
            // Case insensitive check for local commands
            for (int i = 0; command_local[i]; i++) {
                command_local[i] = tolower(command_local[i]);
            }
            //printf("%s\n", command_local);
            if (strcmp(command_local, "open") == 0)
            {
                //printf("Opening window\n");
                target = 0;
            }
            else if (strcmp(command_local, "close") == 0)
            {
                //printf("Closing window, current position %d\n");
                target = 100;
            }
            else
            {
                printf(">> Invalid command\n");
            }
        } else {
           // printf("No input available.\n"); // Optional.  Don't spam this if you really want it non-blocking.
        }

        // Update the position and send DDS update if moving
        if (sample->position != target)
        {
            short delta = (sample->position > target) ? -1 : 1;
            sample->position += delta;
            sample->id = DDS_String_dup(window_id);

            retcode = WindowUpdateDataWriter_write(
                hw_datawriter,
                sample,
                &DDS_HANDLE_NIL);
            if (retcode != DDS_RETCODE_OK)
            {
                printf("Failed to write sample\n");
            } 
            else
            {
                //printf("Written sample %d\n",(int)sample->position);
            } 
        }

        OSAPI_Thread_sleep((RTI_UINT32)application->sleep_time);
    }

    ret_value = 0;

    done:

    #ifndef RTI_CERT
    if (application != NULL)
    {
        Application_delete(application);
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

#if !(defined(RTI_VXWORKS) && !defined(__RTP__))
int
main(int argc, char **argv)
{
    DDS_Long i = 0;
    DDS_Long domain_id = 0;
    char *peer = NULL;
    char *udp_intf = NULL;
    DDS_Long sleep_time = 50;
    DDS_Long count = 0;
    char *window_id = "XY";

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
        else if (!strcmp(argv[i], "-count"))
        {
            ++i;
            if (i == argc)
            {
                printf("-count <count>\n");
                return -1;
            }
            count = (DDS_Long)strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-id"))
        {
            ++i;
            if (i == argc)
            {
                printf("-id <window_id>\n");
                return -1;
            }
            window_id = argv[i];
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

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count, window_id);
}
#elif defined(RTI_VXWORKS)
int
publisher_main(void)
{
    /* Explicitly configure args below */
    DDS_Long domain_id = 0;
    char *peer = "127.0.0.1";
    char *udp_intf = NULL;
    DDS_Long sleep_time = 1000;
    DDS_Long count = 0;

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#endif
