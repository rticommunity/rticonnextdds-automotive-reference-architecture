import tkinter as tk
from tkinter import ttk


class CockpitGUI:

    def __init__(self):
        self.root = tk.Tk()
        self.window_widgets = {}

        self._create_widgets()

        self.root.title("Cockpit")
        self.root.protocol("WM_DELETE_WINDOW", self.root.destroy)


    def _create_widgets(self):
        
        frame_top = ttk.Frame(self.root)
        label = ttk.Label(frame_top, text="Window ids: ")
        entry = ttk.Entry(frame_top)
        entry.insert(0, "FL,FR,RL,RR")
        entry.config(state="readonly")
        label.pack(side=tk.LEFT, padx=5)
        entry.pack(side=tk.LEFT, padx=5)
        frame_top.pack(pady=5)

        frame_bottom = ttk.Frame(self.root)
        # Create a style for the progress bars
        style = ttk.Style()
        style.configure("Vertical.TProgressbar", thickness=100)  # Set the width here
        
        for window_id in entry.get().strip().split(","):
            self.window_widgets[window_id] = dict()
            self.window_widgets[window_id]["pbar_frame"] = pbar_frame = ttk.Frame(frame_bottom)
            self.window_widgets[window_id]["pbar"] = pbar = ttk.Progressbar(pbar_frame, orient="vertical", length=200, style="Vertical.TProgressbar", mode="determinate")
            self.window_widgets[window_id]["btns_frame"] = btns_frame = ttk.Frame(frame_bottom)
            self.window_widgets[window_id]["btn_up"] =   btn_up =   ttk.Button(btns_frame, text="UP (close)",  command=lambda id=window_id: self.window_close(id))
            self.window_widgets[window_id]["btn_down"] = btn_down = ttk.Button(btns_frame, text="DOWN (open)", command=lambda id=window_id: self.window_open(id) )
            pbar.bind("<Button-1>", lambda event, id=window_id: self._pbar_on_click(event, id))
            pbar.pack(side=tk.LEFT, padx=5, pady=5)
            pbar_frame.pack(side=tk.LEFT, padx=5, pady=5)
            btn_up.pack(padx=5, pady=5)
            btn_down.pack(padx=5, pady=5)
            btns_frame.pack(side=tk.LEFT, padx=5, pady=5)
        frame_bottom.pack(pady=5)


    def _pbar_on_click(self, event, window_id):
        pbar = self.window_widgets[window_id]["pbar"]
        pbar_height = pbar.winfo_height()
        click_position = event.y
        new_value = 100 - int((click_position / pbar_height) * 100)
        self.window_set(window_id, new_value)
    

    def start(self):
        self.root.mainloop()
    

    def window_open(self, window_id):
        print(f"Opening window {window_id}")
    

    def window_close(self, window_id):
        print(f"Closing window {window_id}")
    

    def window_set(self, window_id, position):
        print(f"Setting window {window_id} to {position}")
    

    def update_position(self, window_id, position):
        self.window_widgets[window_id]["pbar"]["value"] = position
