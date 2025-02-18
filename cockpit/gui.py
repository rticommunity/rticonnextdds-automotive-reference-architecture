"""GUI for the Cockpit application.

The CockpitGUI class provides a simple GUI for the Cockpit application,
which allows the user to control the position of four windows in the 
window controller example system.

The GUI consists of four vertical progress bars, each representing a window,
and two buttons for each window to open and close it.

Additionally, the user can click on a progress bar to set the position of the
corresponding window.

The CockpitGUI class is designed to be used in conjunction with the CockpitConnext
class, which provides the DDS communication layer for the Cockpit application.

The CockpitApp is the higher level class that ties the CockpitConnext and CockpitGUI
classes together, and provides the main application logic.
"""


import tkinter as tk
from tkinter import ttk



class CockpitGUI:
    """Simple GUI for the Cockpit application, part of the window
    control example system."""

    def __init__(self):
        """Create the GUI window and widgets."""

        self.root = tk.Tk()
        self.window_widgets = {}

        self._create_widgets()

        self.root.title("Cockpit")
        self.root.protocol("WM_DELETE_WINDOW", self.root.destroy)
    

    def start(self):
        """Start the GUI main loop."""
        self.root.mainloop()
    

    def window_open(self, window_id):
        """Dummy function, to be overridden by application."""
        print(f"Opening window {window_id}")
    

    def window_close(self, window_id):
        """Dummy function, to be overridden by application."""
        print(f"Closing window {window_id}")
    

    def window_set(self, window_id, position):
        """Dummy function, to be overridden by application."""
        print(f"Setting window {window_id} to {position}")
    

    def update_position(self, window_id, position):
        """Update the position of a window in the GUI."""
        self.window_widgets[window_id]["pbar"]["value"] = position


    def _create_widgets(self):
        """Create the widgets for the GUI."""
        
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
        """Event handler to trigger a window command when clicking on the progress bar."""

        pbar = self.window_widgets[window_id]["pbar"]
        pbar_height = pbar.winfo_height()
        click_position = event.y
        new_value = 100 - int((click_position / pbar_height) * 100)
        self.window_set(window_id, new_value)