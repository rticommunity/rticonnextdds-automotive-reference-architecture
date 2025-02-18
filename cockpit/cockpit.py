import rti.connextdds as dds
import threading

import Window


class Cockpit:

    DOMAIN_ID = 0

    def __init__(self):
        """Set up the DDS instances."""

        self.sample_update = Window.WindowUpdate()
        
        # DDS instances
        self.participant = dds.DomainParticipant(self.DOMAIN_ID)
        self.topic_update = dds.Topic(self.participant, Window.TOPIC_WINDOW_UPDATE, Window.WindowUpdate)
        self.topic_command = dds.Topic(self.participant, Window.TOPIC_WINDOW_COMMAND, Window.WindowCommand)
        self.writer = dds.DataWriter(self.topic_command)
        # Create a DataReader with Transient Local Durability
        datareader_qos = dds.DataReaderQos()
        datareader_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
        datareader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
        datareader_qos.history.kind = dds.HistoryKind.KEEP_LAST
        datareader_qos.history.depth = 1
        self.reader = dds.DataReader(self.topic_update, datareader_qos)

        # Condition to stop the reader waitset thread
        self.stop_condition = dds.GuardCondition()
        self.read_condition = dds.ReadCondition(self.reader, 
                                                dds.DataState(dds.SampleState.NOT_READ, 
                                                              dds.ViewState.ANY, 
                                                              dds.InstanceState.ANY) )
        self.waitset = dds.WaitSet()
        self.waitset.attach_condition(self.stop_condition)
        self.waitset.attach_condition(self.read_condition)

        self.thread_reader = threading.Thread(target=self.reader_monitor)
        self.thread_reader.start()
    

    def act_on_samples(self, samples):
        "Dummy function, to be overridden by application."
        print(samples)
    

    def reader_monitor(self):
        "Dedicated thread target for subscribing to updates on the reader topic."

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
    

    def send_target(self, window_id, target):
        sample_command = Window.WindowCommand()
        sample_command.id = window_id
        sample_command.position = target 
        self.writer.write(sample_command)
    

    def send_open(self, window_id):
        self.send_target(window_id, 0)

    def send_close(self, window_id):
        self.send_target(window_id, 100)
    

    def close(self):
        "Graceful exit."

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




if __name__ == "__main__":
    app = Cockpit()
        
    import gui

    guiapp = gui.CockpitGUI()

    def act_on_samples(samples):
        for sample in samples:
            guiapp.update_position(sample.id, sample.position)
    app.act_on_samples = act_on_samples

    guiapp.window_open = app.send_open
    guiapp.window_close = app.send_close
    guiapp.window_set = app.send_target

    guiapp.start()

    app.close()