/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from Window.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Data Distribution Service manual.
*/

#ifndef WindowSupport_440091131_h
#define WindowSupport_440091131_h

#include <stdlib.h>

/* Uses */
#include "Window.h"
/* Requires */
#include "WindowPlugin.h"

/* ========================================================================== */
/**
Uses:     T
Defines:  TTypeSupport, TDataWriter, TDataReader*/

#if (defined(RTI_WIN32) || defined(RTI_WIN64) || defined(RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols. */
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
    #endif

    NDDSUSERDllExport extern DDS_ReturnCode_t
    WindowCommandTypeSupport_register_type(
        DDS_DomainParticipant* participant,
        const char* type_name);

    #ifndef RTI_CERT
    NDDSUSERDllExport extern DDS_ReturnCode_t
    WindowCommandTypeSupport_unregister_type(
        DDS_DomainParticipant* participant,
        const char* type_name);
    #endif

    NDDSUSERDllExport extern const char*
    WindowCommandTypeSupport_get_type_name(void);

    NDDSUSERDllExport extern WindowCommand *
    WindowCommandTypeSupport_create_data(void);

    #ifndef RTI_CERT
    NDDSUSERDllExport extern void
    WindowCommandTypeSupport_delete_data(
        WindowCommand *data);
    #endif

    DDS_DATAWRITER_C(WindowCommandDataWriter, WindowCommand);

    DDS_DATAREADER_C(WindowCommandDataReader, WindowCommandSeq, WindowCommand);

    #ifdef __cplusplus
}
#endif

#if (defined(RTI_WIN32) || defined(RTI_WIN64) || defined(RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols. */
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

/* ========================================================================== */
/**
Uses:     T
Defines:  TTypeSupport, TDataWriter, TDataReader*/

#if (defined(RTI_WIN32) || defined(RTI_WIN64) || defined(RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols. */
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
    #endif

    NDDSUSERDllExport extern DDS_ReturnCode_t
    WindowUpdateTypeSupport_register_type(
        DDS_DomainParticipant* participant,
        const char* type_name);

    #ifndef RTI_CERT
    NDDSUSERDllExport extern DDS_ReturnCode_t
    WindowUpdateTypeSupport_unregister_type(
        DDS_DomainParticipant* participant,
        const char* type_name);
    #endif

    NDDSUSERDllExport extern const char*
    WindowUpdateTypeSupport_get_type_name(void);

    NDDSUSERDllExport extern WindowUpdate *
    WindowUpdateTypeSupport_create_data(void);

    #ifndef RTI_CERT
    NDDSUSERDllExport extern void
    WindowUpdateTypeSupport_delete_data(
        WindowUpdate *data);
    #endif

    DDS_DATAWRITER_C(WindowUpdateDataWriter, WindowUpdate);

    DDS_DATAREADER_C(WindowUpdateDataReader, WindowUpdateSeq, WindowUpdate);

    #ifdef __cplusplus
}
#endif

#if (defined(RTI_WIN32) || defined(RTI_WIN64) || defined(RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols. */
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif  /* WindowSupport_440091131_h */

