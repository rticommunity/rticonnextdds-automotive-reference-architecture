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

        self.sample_update = WindowUpdate()
        
        # DDS instances
        self.participant = dds.DomainParticipant(self.DOMAIN_ID)
        self.topic_update = dds.Topic(self.participant, TOPIC_WINDOW_UPDATE, WindowUpdate)
        self.topic_command = dds.Topic(self.participant, TOPIC_WINDOW_COMMAND, WindowCommand)
        self.writer = dds.DataWriter(self.topic_command)
        self.reader = dds.DataReader(self.topic_update)

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
    

    def _send_target(self, window_id, target):
        sample_command = WindowCommand()
        sample_command.id = window_id
        sample_command.position = target 
        self.writer.write(sample_command)
    

    def send_open(self, window_id):
        self._send_target(window_id, 0)

    def send_close(self, window_id):
        self._send_target(window_id, 100)



if __name__ == "__main__":
    app = Cockpit()
    USE_GUI = False

    if USE_GUI:
        import tkinter as tk
        from tkinter import ttk
        root = tk.Tk()
        frame = tk.Frame(root)
        label = tk.Label(frame, text="Window id: ")
        entry = tk.Entry(frame, text="FR")
        btn_open = tk.Button(frame, text="OPEN", command=lambda: app.send_open(entry.get()))
        btn_close = tk.Button(frame, text="CLOSE", command=lambda: app.send_close(entry.get()))
        label.pack(side=tk.LEFT)
        entry.pack(side=tk.LEFT)
        btn_open.pack(side=tk.LEFT)
        btn_close.pack(side=tk.LEFT)
        frame.pack()
        root.mainloop()
    
    else:
        command_format = "<command> <window_id>\n where <command> = open, close"
        print(f"Enter window command: {command_format}")
        print("Enter Ctrl-C to exit")
        try:
            while True:
                command_str = input()
                try:
                    command, window_id = command_str.split(" ")
                except ValueError:
                    print(f">> Error: expected format {command_format}")
                    continue
                try:
                    getattr(app, f"send_{command}")(window_id)
                except AttributeError:
                    print(">> Invalid command")
        except KeyboardInterrupt:
            pass