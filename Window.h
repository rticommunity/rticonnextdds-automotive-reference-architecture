/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from Window.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Data Distribution Service manual.
*/

#ifndef Window_440091058_h
#define Window_440091058_h

#ifndef rti_me_c_h
#include "rti_me_c.h"
#endif

#if (defined(RTI_WIN32) || defined(RTI_WIN64) || defined(RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols. */
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
    #endif

    #define TOPIC_WINDOW_COMMAND ("WindowCommand")

    #define TOPIC_WINDOW_UPDATE ("WindowUpdate")

    typedef struct WindowCommand

    {

        DDS_String id;
        DDS_UnsignedShort position;

    } WindowCommand ;

    NDDSUSERDllExport extern const char *WindowCommandTYPENAME;

    #define REDA_SEQUENCE_USER_API
    #define T WindowCommand
    #define TSeq WindowCommandSeq
    #define REDA_SEQUENCE_EXCLUDE_C_METHODS
    #define REDA_SEQUENCE_USER_CPP
    #include <reda/reda_sequence_decl.h>

    #define REDA_SEQUENCE_USER_API
    #define T WindowCommand
    #define TSeq WindowCommandSeq
    #define REDA_SEQUENCE_EXCLUDE_STRUCT
    #define REDA_SEQUENCE_USER_CPP
    #include <reda/reda_sequence_decl.h>

    NDDSUSERDllExport extern RTI_BOOL
    WindowCommand_initialize(WindowCommand* sample);

    NDDSUSERDllExport extern WindowCommand*
    WindowCommand_create(void);

    #ifndef RTI_CERT
    NDDSUSERDllExport extern RTI_BOOL
    WindowCommand_finalize(WindowCommand* sample);

    NDDSUSERDllExport extern void
    WindowCommand_delete(WindowCommand* sample);
    #endif

    NDDSUSERDllExport extern RTI_BOOL
    WindowCommand_copy(WindowCommand* dst, const WindowCommand* src);

    typedef struct WindowUpdate

    {

        DDS_String id;
        DDS_UnsignedShort position;

    } WindowUpdate ;

    NDDSUSERDllExport extern const char *WindowUpdateTYPENAME;

    #define REDA_SEQUENCE_USER_API
    #define T WindowUpdate
    #define TSeq WindowUpdateSeq
    #define REDA_SEQUENCE_EXCLUDE_C_METHODS
    #define REDA_SEQUENCE_USER_CPP
    #include <reda/reda_sequence_decl.h>

    #define REDA_SEQUENCE_USER_API
    #define T WindowUpdate
    #define TSeq WindowUpdateSeq
    #define REDA_SEQUENCE_EXCLUDE_STRUCT
    #define REDA_SEQUENCE_USER_CPP
    #include <reda/reda_sequence_decl.h>

    NDDSUSERDllExport extern RTI_BOOL
    WindowUpdate_initialize(WindowUpdate* sample);

    NDDSUSERDllExport extern WindowUpdate*
    WindowUpdate_create(void);

    #ifndef RTI_CERT
    NDDSUSERDllExport extern RTI_BOOL
    WindowUpdate_finalize(WindowUpdate* sample);

    NDDSUSERDllExport extern void
    WindowUpdate_delete(WindowUpdate* sample);
    #endif

    NDDSUSERDllExport extern RTI_BOOL
    WindowUpdate_copy(WindowUpdate* dst, const WindowUpdate* src);

    #if (defined(RTI_WIN32) || defined(RTI_WIN64) || defined(RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
    /* If the code is building on Windows, stop exporting symbols. */
    #undef NDDSUSERDllExport
    #define NDDSUSERDllExport
    #endif

    #ifdef __cplusplus
}
#endif

#endif /* Window */

