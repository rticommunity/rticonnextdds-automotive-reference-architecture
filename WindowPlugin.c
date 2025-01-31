/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from Window.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Data Distribution Service manual.
*/

#include "Window.h"
#include "WindowPlugin.h"

/*** SOURCE_BEGIN ***/
#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void)(x)
#endif

/* --------------------------------------------------------------------------
(De)Serialize functions:
* -------------------------------------------------------------------------- */
RTI_BOOL 
WindowCommand_cdr_serialize(
    struct CDR_Stream_t *stream, const void *void_sample, void *param)
{
    WindowCommand *sample = (WindowCommand *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);

    if (!CDR_Stream_serialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    if (!CDR_Stream_serialize_unsigned_short(
        stream, &sample->position))
    {
        return RTI_FALSE;
    }  

    return RTI_TRUE;
}

RTI_BOOL 
WindowCommand_cdr_deserialize(
    struct CDR_Stream_t *stream, void *void_sample, void *param)
{
    WindowCommand *sample = (WindowCommand *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);

    if (!CDR_Stream_deserialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    if (!CDR_Stream_deserialize_unsigned_short(
        stream, &sample->position))
    {
        return RTI_FALSE;
    }  

    return RTI_TRUE;

}

RTI_UINT32
WindowCommand_get_serialized_sample_max_size(
    struct NDDS_Type_Plugin *plugin,
    RTI_UINT32 current_alignment,
    void *param)
{
    RTI_UINT32 initial_alignment = current_alignment;

    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    current_alignment += CDR_get_max_size_serialized_string(
        current_alignment, (4L)+1);

    current_alignment += CDR_get_max_size_serialized_unsigned_short(
        current_alignment);

    return  current_alignment - initial_alignment;
}
/* --------------------------------------------------------------------------
Key Management functions:
* -------------------------------------------------------------------------- */

RTI_BOOL
WindowCommand_cdr_serialize_key(
    struct CDR_Stream_t *stream, const void *void_sample, void *param)
{
    const WindowCommand *sample = (WindowCommand *)void_sample;
    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);
    if (!CDR_Stream_serialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }

    return RTI_TRUE;
}

RTI_BOOL
WindowCommand_cdr_deserialize_key(
    struct CDR_Stream_t *stream, void *void_sample, void *param)
{
    WindowCommand *sample = (WindowCommand *)void_sample;
    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);
    if (!CDR_Stream_deserialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }

    return RTI_TRUE;
}

RTI_UINT32 
WindowCommand_get_serialized_key_max_size(
    struct NDDS_Type_Plugin *plugin,
    RTI_UINT32 current_alignment,
    void *param)
{
    RTI_UINT32 initial_alignment = current_alignment;

    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    current_alignment +=  CDR_get_max_size_serialized_string(
        current_alignment , (4L)+1);

    return current_alignment - initial_alignment;
}

/* --------------------------------------------------------------------------
*  Sample Support functions:
* -------------------------------------------------------------------------- */
RTI_BOOL
WindowCommandPlugin_create_sample(
    struct NDDS_Type_Plugin *plugin, void **sample, void *param)
{
    UNUSED_ARG(plugin);
    UNUSED_ARG(param);

    if (sample == NULL)
    {
        return RTI_FALSE;
    }

    *sample = (void *) WindowCommand_create();

    return (*sample != NULL);
}

#ifndef RTI_CERT
RTI_BOOL
WindowCommandPlugin_delete_sample(
    struct NDDS_Type_Plugin *plugin, void *sample, void *param)
{
    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    /* WindowCommand_delete() is a void function
    * which expects (sample != NULL). Since 
    * WindowCommandPlugin_delete_sample
    * is an internal function, sample is assumed to be a valid pointer 
    */ 
    WindowCommand_delete((WindowCommand *) sample);
    return RTI_TRUE;
}
#endif

RTI_BOOL 
WindowCommandPlugin_copy_sample(
    struct NDDS_Type_Plugin *plugin, void *dst, const void *src, void *param)
{
    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    return WindowCommand_copy(
        (WindowCommand *)dst,
        (const WindowCommand *)src);
}
/* --------------------------------------------------------------------------
*  Type WindowCommand Plugin Instantiation
* -------------------------------------------------------------------------- */

NDDSCDREncapsulation WindowCommandEncapsulationKind[] =
{ {0,0} };

struct NDDS_Type_Plugin WindowCommandTypePlugin =
{
    {0, 0},                     /* NDDS_Type_PluginVersion */
    NULL,                       /* DDS_TypeCode_t* */
    WindowCommandEncapsulationKind,
    NDDS_TYPEPLUGIN_USER_KEY,   /* NDDS_TypePluginKeyKind */
    WindowCommand_cdr_serialize,
    WindowCommand_cdr_deserialize,
    WindowCommand_get_serialized_sample_max_size,
    WindowCommand_cdr_serialize_key,
    WindowCommand_cdr_deserialize_key,
    WindowCommand_get_serialized_key_max_size,
    WindowCommandPlugin_create_sample,
    #ifndef RTI_CERT
    WindowCommandPlugin_delete_sample,
    #else
    NULL,
    #endif
    WindowCommandPlugin_copy_sample,
    PluginHelper_get_key_kind,
    PluginHelper_instance_to_keyhash,
    NULL, NULL, NULL, NULL  /* endpoint wrappers not used in C */
};

/* --------------------------------------------------------------------------
*  Type WindowCommand Plugin Methods
* -------------------------------------------------------------------------- */

struct NDDS_Type_Plugin *
WindowCommandTypePlugin_get(void) 
{ 
    return &WindowCommandTypePlugin;
} 

const char*
WindowCommandTypePlugin_get_default_type_name(void) 
{ 
    return WindowCommandTYPENAME;
} 

NDDS_TypePluginKeyKind 
WindowCommand_get_key_kind(
    struct NDDS_Type_Plugin *plugin,
    void *param)
{
    UNUSED_ARG(param);
    UNUSED_ARG(plugin);
    return NDDS_TYPEPLUGIN_USER_KEY;
}

/* --------------------------------------------------------------------------
(De)Serialize functions:
* -------------------------------------------------------------------------- */
RTI_BOOL 
WindowUpdate_cdr_serialize(
    struct CDR_Stream_t *stream, const void *void_sample, void *param)
{
    WindowUpdate *sample = (WindowUpdate *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);

    if (!CDR_Stream_serialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    if (!CDR_Stream_serialize_unsigned_short(
        stream, &sample->position))
    {
        return RTI_FALSE;
    }  

    return RTI_TRUE;
}

RTI_BOOL 
WindowUpdate_cdr_deserialize(
    struct CDR_Stream_t *stream, void *void_sample, void *param)
{
    WindowUpdate *sample = (WindowUpdate *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);

    if (!CDR_Stream_deserialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    if (!CDR_Stream_deserialize_unsigned_short(
        stream, &sample->position))
    {
        return RTI_FALSE;
    }  

    return RTI_TRUE;

}

RTI_UINT32
WindowUpdate_get_serialized_sample_max_size(
    struct NDDS_Type_Plugin *plugin,
    RTI_UINT32 current_alignment,
    void *param)
{
    RTI_UINT32 initial_alignment = current_alignment;

    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    current_alignment += CDR_get_max_size_serialized_string(
        current_alignment, (4L)+1);

    current_alignment += CDR_get_max_size_serialized_unsigned_short(
        current_alignment);

    return  current_alignment - initial_alignment;
}
/* --------------------------------------------------------------------------
Key Management functions:
* -------------------------------------------------------------------------- */

RTI_BOOL
WindowUpdate_cdr_serialize_key(
    struct CDR_Stream_t *stream, const void *void_sample, void *param)
{
    const WindowUpdate *sample = (WindowUpdate *)void_sample;
    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);
    if (!CDR_Stream_serialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }

    return RTI_TRUE;
}

RTI_BOOL
WindowUpdate_cdr_deserialize_key(
    struct CDR_Stream_t *stream, void *void_sample, void *param)
{
    WindowUpdate *sample = (WindowUpdate *)void_sample;
    if ((stream == NULL) || (void_sample == NULL))
    {
        return RTI_FALSE;
    }

    UNUSED_ARG(param);
    if (!CDR_Stream_deserialize_string(
        stream,
        sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }

    return RTI_TRUE;
}

RTI_UINT32 
WindowUpdate_get_serialized_key_max_size(
    struct NDDS_Type_Plugin *plugin,
    RTI_UINT32 current_alignment,
    void *param)
{
    RTI_UINT32 initial_alignment = current_alignment;

    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    current_alignment +=  CDR_get_max_size_serialized_string(
        current_alignment , (4L)+1);

    return current_alignment - initial_alignment;
}

/* --------------------------------------------------------------------------
*  Sample Support functions:
* -------------------------------------------------------------------------- */
RTI_BOOL
WindowUpdatePlugin_create_sample(
    struct NDDS_Type_Plugin *plugin, void **sample, void *param)
{
    UNUSED_ARG(plugin);
    UNUSED_ARG(param);

    if (sample == NULL)
    {
        return RTI_FALSE;
    }

    *sample = (void *) WindowUpdate_create();

    return (*sample != NULL);
}

#ifndef RTI_CERT
RTI_BOOL
WindowUpdatePlugin_delete_sample(
    struct NDDS_Type_Plugin *plugin, void *sample, void *param)
{
    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    /* WindowUpdate_delete() is a void function
    * which expects (sample != NULL). Since 
    * WindowUpdatePlugin_delete_sample
    * is an internal function, sample is assumed to be a valid pointer 
    */ 
    WindowUpdate_delete((WindowUpdate *) sample);
    return RTI_TRUE;
}
#endif

RTI_BOOL 
WindowUpdatePlugin_copy_sample(
    struct NDDS_Type_Plugin *plugin, void *dst, const void *src, void *param)
{
    UNUSED_ARG(plugin);
    UNUSED_ARG(param);
    return WindowUpdate_copy(
        (WindowUpdate *)dst,
        (const WindowUpdate *)src);
}
/* --------------------------------------------------------------------------
*  Type WindowUpdate Plugin Instantiation
* -------------------------------------------------------------------------- */

NDDSCDREncapsulation WindowUpdateEncapsulationKind[] =
{ {0,0} };

struct NDDS_Type_Plugin WindowUpdateTypePlugin =
{
    {0, 0},                     /* NDDS_Type_PluginVersion */
    NULL,                       /* DDS_TypeCode_t* */
    WindowUpdateEncapsulationKind,
    NDDS_TYPEPLUGIN_USER_KEY,   /* NDDS_TypePluginKeyKind */
    WindowUpdate_cdr_serialize,
    WindowUpdate_cdr_deserialize,
    WindowUpdate_get_serialized_sample_max_size,
    WindowUpdate_cdr_serialize_key,
    WindowUpdate_cdr_deserialize_key,
    WindowUpdate_get_serialized_key_max_size,
    WindowUpdatePlugin_create_sample,
    #ifndef RTI_CERT
    WindowUpdatePlugin_delete_sample,
    #else
    NULL,
    #endif
    WindowUpdatePlugin_copy_sample,
    PluginHelper_get_key_kind,
    PluginHelper_instance_to_keyhash,
    NULL, NULL, NULL, NULL  /* endpoint wrappers not used in C */
};

/* --------------------------------------------------------------------------
*  Type WindowUpdate Plugin Methods
* -------------------------------------------------------------------------- */

struct NDDS_Type_Plugin *
WindowUpdateTypePlugin_get(void) 
{ 
    return &WindowUpdateTypePlugin;
} 

const char*
WindowUpdateTypePlugin_get_default_type_name(void) 
{ 
    return WindowUpdateTYPENAME;
} 

NDDS_TypePluginKeyKind 
WindowUpdate_get_key_kind(
    struct NDDS_Type_Plugin *plugin,
    void *param)
{
    UNUSED_ARG(param);
    UNUSED_ARG(plugin);
    return NDDS_TYPEPLUGIN_USER_KEY;
}

