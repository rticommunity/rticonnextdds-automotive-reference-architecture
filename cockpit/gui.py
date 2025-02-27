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

import os
import tkinter as tk
from tkinter import ttk



class CustomVerticalProgressBar(tk.Canvas):
    def __init__(self, parent, width=300, height=180, progress=0, **kwargs):
        super().__init__(parent, width=width, height=height, **kwargs)
        self.width = width
        self.height = height
        self.progress = progress
        self.base_x = self.base_y = 2
        self.rect = self.create_rectangle(self.base_x, self.base_y, width, height, fill="lightblue", outline="grey")
        self.set_progress(self.progress)

    def set_progress(self, value):
        self.progress = value
        # Adjust so outline is visible when fully filled
        base_y = self.base_y if value==100 else self.height - (self.height * (value / 100))
        self.coords(self.rect, self.base_x, base_y, self.width, self.height)



class CockpitGUI:
    """Simple GUI for the Cockpit application, part of the window
    control example system."""

    def __init__(self, window_ids=["FL", "FR", "RL", "RR"]):
        """Create the GUI window and widgets."""

        self.window_ids = window_ids

        self.root = tk.Tk()
        self.window_widgets = {window_id: {} for window_id in self.window_ids}

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
        self.window_widgets[window_id]["pbar"].set_progress(position)


    def _create_widgets(self):
        """Create the widgets for the GUI."""
        
        # Load the background image
        img_path = os.path.join(os.path.dirname(__file__), "infotainment.png")
        self.background_image = tk.PhotoImage(file=img_path)
        self.background_label = tk.Label(self.root, image=self.background_image)
        self.background_label.place(relwidth=1, relheight=1)

        # Set the window size to match the image size
        self.root.geometry(f"{self.background_image.width()}x{self.background_image.height()}")
        self.root.resizable(False, False)

        frame = ttk.Frame(self.root, width=762, height=504)
        frame.grid_propagate(False)  # Prevent the frame from resizing to fit its children

        # Add column weights to columns 0, 1, 2, and 3
        for i in range(4):
            frame.columnconfigure(i, weight=1)
        frame.grid_columnconfigure(1, weight=10)
        frame.grid_rowconfigure(2, weight=10)

        # Create the label and pack it inside the label frame
        label = ttk.Label(frame, text="Window Controls", font=("Arial", 22, "bold"), anchor=tk.CENTER)
        label.grid(row=0, column=0, columnspan=4, sticky=tk.EW)

        # Create a style for the progress bars
        #style_pbar = ttk.Style()
        #style_pbar.configure("Vertical.TProgressbar", thickness=300)  # Set the width here

        def _create_pbar_and_buttons(window_id, parent=frame):
            frame = ttk.Frame(parent)
            label = ttk.Label(frame, text=window_id, anchor=tk.CENTER, font=("Arial", 16, "bold"))
            pbar = CustomVerticalProgressBar(frame)
            btns_frame = ttk.Frame(frame)
            btn_up = ttk.Button(btns_frame, text="^", command=lambda: self.window_close(window_id), width=2)
            btn_spacer = ttk.Frame(btns_frame)
            btn_down = ttk.Button(btns_frame, text="v", command=lambda: self.window_open(window_id), width=2)
            pbar.bind("<Button-1>", lambda event: self._pbar_on_click(event, window_id))
            
            label.grid(row=0, column=0, columnspan=2, sticky=tk.EW)
            pbar.grid(row=1, column=0, sticky=tk.NSEW, padx=5)
            btns_frame.grid(row=1, column=1, sticky=tk.NSEW)
            
            btn_up.pack()
            btn_spacer.pack(fill=tk.Y, expand=True)
            btn_down.pack()
            self.window_widgets[window_id]["pbar"] = pbar
            return frame

        for i, window_id in enumerate(self.window_ids):
            # Place the progress bars and buttons in a 2x2-like grid, starting
            # from row 1. (given row is has the title label)
            # Note: row 2 and column 1 are reserved for expanding spacers.
            pbar_and_buttons_frame = _create_pbar_and_buttons(window_id, parent=frame)
            pbar_and_buttons_frame.grid(row=(i//2+1)*2, column=(i%2)*2, padx=5, pady=5, sticky=tk.NSEW)
        
        hspacer = ttk.Frame(frame)
        hspacer.grid(row=2, column=1, padx=5, pady=5, sticky=tk.E+tk.W)
        vspacer = ttk.Frame(frame)
        vspacer.grid(row=2, column=1, padx=5, pady=5, sticky=tk.N+tk.S)
        
        # Match the position of the frame to the position of the screen in the background image
        frame.place(x=127, y=100)

        # Lower the background picture to the bottom layer in the GUI
        self.background_label.lower()


    def _pbar_on_click(self, event, window_id):
        """Event handler to trigger a window command when clicking on the progress bar."""

        pbar = self.window_widgets[window_id]["pbar"]
        pbar_height = pbar.height
        click_position = event.y
        new_value = 100 - int((click_position / pbar_height) * 100)
        self.window_set(window_id, new_value)