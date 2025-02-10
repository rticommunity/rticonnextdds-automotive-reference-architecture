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
    

    def _send_target(self, window_id, target):
        sample_command = WindowCommand()
        sample_command.id = window_id
        sample_command.position = target 
        self.writer.write(sample_command)
    

    def send_open(self, window_id):
        self._send_target(window_id, 0)

    def send_close(self, window_id):
        self._send_target(window_id, 100)
    

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
    USE_GUI = True

    if USE_GUI:
        import tkinter as tk
        from tkinter import ttk
        root = tk.Tk()
        frame_top = ttk.Frame(root)
        label = ttk.Label(frame_top, text="Window ids: ")
        entry = ttk.Entry(frame_top)
        entry.insert(0, "FL,FR,RL,RR")
        label.pack(side=tk.LEFT, padx=5)
        entry.pack(side=tk.LEFT, padx=5)
        frame_top.pack(pady=5)

        frame_bottom = ttk.Frame(root)
        # Create a style for the progress bars
        style = ttk.Style()
        style.configure("Vertical.TProgressbar", thickness=100)  # Set the width here
        window_widgets = {}
        for window_id in entry.get().strip().split(","):
            window_widgets[window_id] = dict()
            window_widgets[window_id]["pbar_frame"] = pbar_frame = ttk.Frame(frame_bottom)
            window_widgets[window_id]["pbar"] = pbar = ttk.Progressbar(pbar_frame, orient="vertical", length=200, style="Vertical.TProgressbar", mode="determinate")
            window_widgets[window_id]["btns_frame"] = btns_frame = ttk.Frame(frame_bottom)
            window_widgets[window_id]["btn_up"] =   btn_up =   ttk.Button(btns_frame, text="UP (close)",  command=lambda id=window_id: app.send_close(id))
            window_widgets[window_id]["btn_down"] = btn_down = ttk.Button(btns_frame, text="DOWN (open)", command=lambda id=window_id: app.send_open(id) )
            pbar.pack(side=tk.LEFT, padx=5, pady=5)
            pbar_frame.pack(side=tk.LEFT, padx=5, pady=5)
            btn_up.pack(padx=5, pady=5)
            btn_down.pack(padx=5, pady=5)
            btns_frame.pack(side=tk.LEFT, padx=5, pady=5)
        frame_bottom.pack(pady=5)

        def act_on_samples(samples):
            for sample in samples:
                window_widgets[sample.id]["pbar"]["value"] = sample.position
        
        app.act_on_samples = act_on_samples

        root.mainloop()

        app.close()
    
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
            app.close()