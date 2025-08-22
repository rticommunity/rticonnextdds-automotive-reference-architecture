/*
 * (c) 2025 Copyright, Real-Time Innovations, Inc. All rights reserved.
 * No duplications, whole or partial, manual or electronic, may be made
 * without express written permission.  Any such copies, or revisions thereof,
 * must display this notice unaltered.
 * This code contains trade secrets of Real-Time Innovations, Inc.
 */

#ifndef Application_h
#define Application_h

#include "rti_me_c.h"

extern void 
Application_help(char *appname);

extern DDS_Boolean
Application_initialize_micro(char *udp_intf);

extern DDS_Topic *
Application_register_type_and_create_topic_window_update(DDS_DomainParticipant *participant);

extern DDS_Topic *
Application_register_type_and_create_topic_window_command(DDS_DomainParticipant *participant);

extern DDS_DomainParticipant *
Application_create_participant(DDS_Long domain_id, char *peer);

#ifndef RTI_CERT
extern void
Application_delete(DDS_DomainParticipant *participant);
#endif

#endif
