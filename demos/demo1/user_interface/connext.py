# (c) 2025 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

"""Connext DDS implementation of the interface for the window control example system.

This module provides the ConnextApp class, which is a wrapper around the Connext DDS API
to provide a simple interface for GUI applications.

The ConnextApp class is responsible for setting up the DDS entities, sending commands
to the window control system, and receiving updates from the window control system.

The ConnextApp class is designed to be used in conjunction with GUI classes such as
CockpitGUI or MobileAppGUI, which provide graphical user interfaces for the application.

The main application logic can be implemented by extending or integrating the ConnextApp
class with the desired GUI framework.
"""

import rti.connextdds as dds
import threading

import Window



class ConnextApp:
    """Wrapper around the Connext DDS API to provide a simple
    interface for the main GUI application."""
    
    def __init__(self, domain_id = 0):
        """Set up the DDS instances and start the data monitoring thread."""

        self.sample_update = Window.WindowUpdate()
        
        # DDS instances - one participant, two topics, one writer, one reader
        self.participant = dds.DomainParticipant(domain_id)
        self.topic_update = dds.Topic(self.participant, Window.TOPIC_WINDOW_UPDATE, Window.WindowUpdate)
        self.topic_command = dds.Topic(self.participant, Window.TOPIC_WINDOW_COMMAND, Window.WindowCommand)
        self.writer = dds.DataWriter(self.topic_command)
        self.reader = dds.DataReader(self.topic_update, self._datareader_qos_transient_local)

        # Use a WaitSet to monitor incoming data
        self.waitset = dds.WaitSet()
        # Conditions to trigger and/or stop the waitset
        self.stop_condition = dds.GuardCondition()
        self.read_condition = dds.ReadCondition(self.reader, 
                                                dds.DataState(dds.SampleState.NOT_READ, 
                                                              dds.ViewState.ANY, 
                                                              dds.InstanceState.ANY) )
        self.waitset.attach_condition(self.stop_condition)
        self.waitset.attach_condition(self.read_condition)
        # Thread to keep the waitset running
        self.thread_reader = threading.Thread(target=self._reader_monitor)
        self.thread_reader.start()
    

    def act_on_samples(self, samples):
        """Dummy function, to be overridden by application."""
        print(samples)
    

    def send_open(self, window_id):
        """Send a command to open the window."""
        self.send_target(window_id, 0)


    def send_close(self, window_id):
        """Send a command to close the window."""
        self.send_target(window_id, 100)
    
    
    def send_target(self, window_id, target):
        """Send a command to set the window to a target position."""
        sample_command = Window.WindowCommand()
        sample_command.id = window_id
        sample_command.position = target 
        self.writer.write(sample_command)
    

    def close(self):
        """Graceful exit."""

        # Early exit if already closed
        if self.participant.closed:
            return

        # Trigger guard condition so waitset threads can finish
        self.stop_condition.trigger_value = True
        # Wait until waitset threads are fully done
        self.thread_reader.join()
        # Detach any remaining conditions
        self.waitset.detach_all()
        # Close entities and participant 
        self.participant.close_contained_entities()
        self.participant.close()
    

    @property
    def _datareader_qos_transient_local(self):
        """Return a DataReaderQoS object with Transient Local Durability settings."""

        datareader_qos = dds.DataReaderQos()
        datareader_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
        datareader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
        datareader_qos.history.kind = dds.HistoryKind.KEEP_LAST
        datareader_qos.history.depth = 1
        return datareader_qos
    

    def _reader_monitor(self):
        """Dedicated thread target for subscribing to updates on the reader topic."""

        while True:
            active_conditions = self.waitset.wait(dds.Duration(1))

            for cond in active_conditions:
                if cond == self.stop_condition:
                    return

                if cond == self.read_condition:
                    # Take any samples that we have not seen before
                    new_samples = self.reader.select().condition(self.read_condition).take_data()
                    # Take action
                    self.act_on_samples(new_samples)