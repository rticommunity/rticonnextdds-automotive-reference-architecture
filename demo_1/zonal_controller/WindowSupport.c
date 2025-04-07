/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from Window.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Data Distribution Service manual.
*/

#include "WindowSupport.h"

/*** SOURCE_BEGIN ***/

/* =========================================================================== */

/* Requires */
#define TTYPENAME   WindowCommandTYPENAME

/* 
WindowCommandDataWriter (DDS_DataWriter)   
*/

/* Defines */
#define TDataWriter WindowCommandDataWriter
#define TData       WindowCommand

#include "dds_c/dds_c_tdatawriter_gen.h"

#undef TDataWriter
#undef TData

/* =========================================================================== */
/* 
WindowCommandDataReader (DDS_DataReader)   
*/

/* Defines */
#define TDataReader WindowCommandDataReader
#define TDataSeq    WindowCommandSeq
#define TData       WindowCommand
#include "dds_c/dds_c_tdatareader_gen.h"
#undef TDataReader
#undef TDataSeq
#undef TData

DDS_ReturnCode_t
WindowCommandTypeSupport_register_type(
    DDS_DomainParticipant* participant,
    const char* type_name)
{
    DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;

    if (participant == NULL) 
    {
        goto done;
    }

    if (type_name == NULL) 
    {
        type_name = WindowCommandTypePlugin_get_default_type_name();
        if (type_name == NULL)
        {
            goto done;
        }
    }

    retcode = DDS_DomainParticipant_register_type(
        participant,
        type_name,
        WindowCommandTypePlugin_get());

    if (retcode != DDS_RETCODE_OK)
    {
        goto done;
    }

    retcode = DDS_RETCODE_OK;

    done:

    return retcode;
}

#ifndef RTI_CERT
DDS_ReturnCode_t
WindowCommandTypeSupport_unregister_type(
    DDS_DomainParticipant* participant,
    const char* type_name)
{
    DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;

    if (participant == NULL) 
    {
        goto done;
    }

    if (type_name == NULL) 
    {
        type_name = WindowCommandTypePlugin_get_default_type_name();
        if (type_name == NULL)
        {
            goto done;
        }
    }

    if (WindowCommandTypePlugin_get() !=
    DDS_DomainParticipant_unregister_type(participant,type_name))
    {
        goto done;
    }

    retcode = DDS_RETCODE_OK;

    done:

    return retcode;
}
#endif
const char*
WindowCommandTypeSupport_get_type_name(void)
{
    return WindowCommandTYPENAME;
}
WindowCommand *
WindowCommandTypeSupport_create_data(void)
{
    WindowCommand *data = NULL;

    data = WindowCommand_create();

    return data;
}

#ifndef RTI_CERT
void
WindowCommandTypeSupport_delete_data(
    WindowCommand *data)
{
    WindowCommand_delete(data);
}
#endif

#undef TTYPENAME

/* =========================================================================== */

/* Requires */
#define TTYPENAME   WindowUpdateTYPENAME

/* 
WindowUpdateDataWriter (DDS_DataWriter)   
*/

/* Defines */
#define TDataWriter WindowUpdateDataWriter
#define TData       WindowUpdate

#include "dds_c/dds_c_tdatawriter_gen.h"

#undef TDataWriter
#undef TData

/* =========================================================================== */
/* 
WindowUpdateDataReader (DDS_DataReader)   
*/

/* Defines */
#define TDataReader WindowUpdateDataReader
#define TDataSeq    WindowUpdateSeq
#define TData       WindowUpdate
#include "dds_c/dds_c_tdatareader_gen.h"
#undef TDataReader
#undef TDataSeq
#undef TData

DDS_ReturnCode_t
WindowUpdateTypeSupport_register_type(
    DDS_DomainParticipant* participant,
    const char* type_name)
{
    DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;

    if (participant == NULL) 
    {
        goto done;
    }

    if (type_name == NULL) 
    {
        type_name = WindowUpdateTypePlugin_get_default_type_name();
        if (type_name == NULL)
        {
            goto done;
        }
    }

    retcode = DDS_DomainParticipant_register_type(
        participant,
        type_name,
        WindowUpdateTypePlugin_get());

    if (retcode != DDS_RETCODE_OK)
    {
        goto done;
    }

    retcode = DDS_RETCODE_OK;

    done:

    return retcode;
}

#ifndef RTI_CERT
DDS_ReturnCode_t
WindowUpdateTypeSupport_unregister_type(
    DDS_DomainParticipant* participant,
    const char* type_name)
{
    DDS_ReturnCode_t retcode = DDS_RETCODE_ERROR;

    if (participant == NULL) 
    {
        goto done;
    }

    if (type_name == NULL) 
    {
        type_name = WindowUpdateTypePlugin_get_default_type_name();
        if (type_name == NULL)
        {
            goto done;
        }
    }

    if (WindowUpdateTypePlugin_get() !=
    DDS_DomainParticipant_unregister_type(participant,type_name))
    {
        goto done;
    }

    retcode = DDS_RETCODE_OK;

    done:

    return retcode;
}
#endif
const char*
WindowUpdateTypeSupport_get_type_name(void)
{
    return WindowUpdateTYPENAME;
}
WindowUpdate *
WindowUpdateTypeSupport_create_data(void)
{
    WindowUpdate *data = NULL;

    data = WindowUpdate_create();

    return data;
}

#ifndef RTI_CERT
void
WindowUpdateTypeSupport_delete_data(
    WindowUpdate *data)
{
    WindowUpdate_delete(data);
}
#endif

#undef TTYPENAME

