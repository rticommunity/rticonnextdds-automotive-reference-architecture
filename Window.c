/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from Window.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Data Distribution Service manual.
*/

#include "Window.h"

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) (void)(x)
#endif

/*** SOURCE_BEGIN ***/

/* ========================================================================= */

const char *WindowCommandTYPENAME = "WindowCommand";

RTI_BOOL
WindowCommand_initialize(WindowCommand* sample)
{
    if (sample == NULL)
    {
        return RTI_FALSE;
    }

    if (!CDR_String_initialize(
        &sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    CDR_Primitive_init_unsigned_short(&sample->position);
    return RTI_TRUE;
}

WindowCommand *
WindowCommand_create(void)
{
    WindowCommand* sample;
    OSAPI_Heap_allocate_struct(&sample, WindowCommand);
    if (sample != NULL)
    {
        if (!WindowCommand_initialize(sample))
        {
            OSAPI_Heap_free_struct(sample);
            sample = NULL;
        }
    }
    return sample;
}

#ifndef RTI_CERT

RTI_BOOL
WindowCommand_finalize(WindowCommand* sample)
{
    if (sample == NULL)
    {
        return RTI_FALSE;
    }

    CDR_String_finalize(&sample->id);
    return RTI_TRUE;
}

#ifndef RTI_CERT
void
WindowCommand_delete(WindowCommand* sample)
{
    if (sample != NULL)
    {
        /* WindowCommand_finalize() always 
        returns RTI_TRUE when called with sample != NULL */
        WindowCommand_finalize(sample);
        OSAPI_Heap_free_struct(sample);
    }
}
#endif
#endif

RTI_BOOL
WindowCommand_copy(WindowCommand* dst,const WindowCommand* src)
{        
    if ((dst == NULL) || (src == NULL))
    {
        return RTI_FALSE;
    }
    if (!CDR_String_copy(
        &dst->id, &src->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    CDR_Primitive_copy_unsigned_short(&dst->position, &src->position);
    return RTI_TRUE;
}

/**
* <<IMPLEMENTATION>>
*
* Defines:  TSeq, T
*
* Configure and implement 'WindowCommand' sequence class.
*/
#define REDA_SEQUENCE_USER_API
#define T WindowCommand
#define TSeq WindowCommandSeq
#define T_initialize WindowCommand_initialize
#define T_finalize   WindowCommand_finalize
#define T_copy       WindowCommand_copy
#include "reda/reda_sequence_defn.h"
#undef T_copy
#undef T_finalize
#undef T_initialize

/* ========================================================================= */

const char *WindowUpdateTYPENAME = "WindowUpdate";

RTI_BOOL
WindowUpdate_initialize(WindowUpdate* sample)
{
    if (sample == NULL)
    {
        return RTI_FALSE;
    }

    if (!CDR_String_initialize(
        &sample->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    CDR_Primitive_init_unsigned_short(&sample->position);
    return RTI_TRUE;
}

WindowUpdate *
WindowUpdate_create(void)
{
    WindowUpdate* sample;
    OSAPI_Heap_allocate_struct(&sample, WindowUpdate);
    if (sample != NULL)
    {
        if (!WindowUpdate_initialize(sample))
        {
            OSAPI_Heap_free_struct(sample);
            sample = NULL;
        }
    }
    return sample;
}

#ifndef RTI_CERT

RTI_BOOL
WindowUpdate_finalize(WindowUpdate* sample)
{
    if (sample == NULL)
    {
        return RTI_FALSE;
    }

    CDR_String_finalize(&sample->id);
    return RTI_TRUE;
}

#ifndef RTI_CERT
void
WindowUpdate_delete(WindowUpdate* sample)
{
    if (sample != NULL)
    {
        /* WindowUpdate_finalize() always 
        returns RTI_TRUE when called with sample != NULL */
        WindowUpdate_finalize(sample);
        OSAPI_Heap_free_struct(sample);
    }
}
#endif
#endif

RTI_BOOL
WindowUpdate_copy(WindowUpdate* dst,const WindowUpdate* src)
{        
    if ((dst == NULL) || (src == NULL))
    {
        return RTI_FALSE;
    }
    if (!CDR_String_copy(
        &dst->id, &src->id,
        (4L)))
    {
        return RTI_FALSE;
    }
    CDR_Primitive_copy_unsigned_short(&dst->position, &src->position);
    return RTI_TRUE;
}

/**
* <<IMPLEMENTATION>>
*
* Defines:  TSeq, T
*
* Configure and implement 'WindowUpdate' sequence class.
*/
#define REDA_SEQUENCE_USER_API
#define T WindowUpdate
#define TSeq WindowUpdateSeq
#define T_initialize WindowUpdate_initialize
#define T_finalize   WindowUpdate_finalize
#define T_copy       WindowUpdate_copy
#include "reda/reda_sequence_defn.h"
#undef T_copy
#undef T_finalize
#undef T_initialize

