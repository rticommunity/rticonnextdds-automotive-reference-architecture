import rti.idl as idl


TOPIC_WINDOW_COMMAND = "WindowCommand"
TOPIC_WINDOW_UPDATE = "WindowUpdate"


@idl.struct(
    member_annotations = {
        'id': [idl.key, idl.bound(4)],
    }
)
class WindowCommand:
    id: str = ""
    position: idl.uint16 = 0


@idl.struct(
    member_annotations = {
        'id': [idl.key, idl.bound(4)],
    }
)
class WindowUpdate:
    id: str = ""
    position: idl.uint16 = 0





import rti.connextdds as dds
import threading


class Cockpit:

    DOMAIN_ID = 0

    def __init__(self):
        """Set up the DDS instances."""

        self.data_sample = WindowUpdate()
        
        # DDS instances
        self.participant = dds.DomainParticipant(self.DOMAIN_ID)
        self.topic = dds.Topic(self.participant, TOPIC_WINDOW_UPDATE, WindowUpdate)
        self.reader = dds.DataReader(self.topic)

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
                    print(new_samples)



if __name__ == "__main__":
    app = Cockpit()