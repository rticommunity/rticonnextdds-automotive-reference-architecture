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



class CustomVerticalBar(tk.Canvas):
    """ Custom vertical bar widget, similar to a progress bar, based on a Canvas.
    Used to represent a window, displaying the position of the window as a filled area. """

    def __init__(self, parent, width=300, height=180, progress=0, **kwargs):
        """Create a vertical bar widget with the specified width, height and progress."""
        super().__init__(parent, width=width, height=height, **kwargs)
        self.width = width
        self.height = height
        self.progress = progress
        self.base_x = self.base_y = 2
        self.rect = self.create_rectangle(self.base_x, self.base_y, width, height, fill="lightblue", outline="grey")
        # Set the progress to the initial value
        self.set_progress(self.progress)

    def set_progress(self, value):
        """Set the progress of the vertical bar to the specified value (0-100)."""
        self.progress = value
        # Adjust so outline is visible when fully filled
        base_y = self.base_y if value==100 else self.height - (self.height * (value / 100))
        self.coords(self.rect, self.base_x, base_y, self.width, self.height)



class CockpitGUI:
    """Simple GUI for the Cockpit application, part of the window control example system."""

    WINDOW_TITLE = "Cockpit"

    def __init__(self, window_ids=["FL", "FR", "RL", "RR"]):
        """Create the GUI window and widgets."""

        self.window_ids = window_ids

        self.root = tk.Tk()
        self.window_widgets = {window_id: {} for window_id in self.window_ids}

        self._create_widgets()

        self.root.title(self.WINDOW_TITLE)
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
        self.window_widgets[window_id]["vbar"].set_progress(position)


    def _create_widgets(self):
        """Create the widgets for the GUI."""
        
        # Load the background image
        script_dir = os.path.dirname(os.path.abspath(__file__))
        img_path = os.path.join(script_dir, "infotainment.png")
        self.background_image = tk.PhotoImage(file=img_path)
        self.background_label = tk.Label(self.root, image=self.background_image)
        self.background_label.place(relwidth=1, relheight=1)

        # Set the window size to match the image size
        self.root.geometry(f"{self.background_image.width()}x{self.background_image.height()}")
        self.root.resizable(False, False)

        # Create a frame to hold all widgets, occupying the screen in the background picture
        frame = ttk.Frame(self.root, width=762, height=504)
        # Match the position of the frame to the position of the screen in the background image
        frame.place(x=127, y=100)
        # Prevent the frame from resizing to fit its children
        frame.grid_propagate(False)
        # Have column 1 and row 2 expand to fill the available space (spacers)
        frame.columnconfigure(1, weight=10)
        frame.rowconfigure(2, weight=10)

        # Label for the screen heading/title
        label_title = ttk.Label(frame, text="Window Controls", font=("Arial", 22, "bold"), anchor=tk.CENTER)

        # Re-format the window_ids to match the desired grid layout (one pair of windows per column)
        window_ids = [self.window_ids[i:i + 2] for i in range(0, len(self.window_ids), 2)]
        # Loop over the list, creating the bars and buttons for each window and placing them in the grid
        for row, column_ids in enumerate(window_ids):
            for col, window_id in enumerate(column_ids):
                vbar_and_buttons_frame = self._create_vbar_and_buttons(window_id, frame)
                # Place the progress bars and buttons in a 2x2-like grid, starting from row 1
                # - row 0 is reserved for the title label (spanning all columns).
                # - row 1 is reserved for a vertically expanding spacer.
                # - column 1 is reserved for a horizontally expanding spacer.
                vbar_and_buttons_frame.grid(row=(row+1)*2, column=col*2, padx=5, pady=5, sticky=tk.NSEW)
        
        # Horizontally expanding spacer in odd columns
        hspacer = ttk.Frame(frame)
        hspacer.grid(row=1, column=1, padx=5, pady=5, sticky=tk.E+tk.W)
        # Vertically expanding spacer in even rows
        vspacer = ttk.Frame(frame)
        vspacer.grid(row=2, column=3, padx=5, pady=5, sticky=tk.N+tk.S)

        # Place the title label in the top row, spanning all columns
        label_title.grid(row=0, column=0, columnspan=4, sticky=tk.EW)

        # Lower the background picture to the bottom layer in the GUI
        self.background_label.lower()
        

    def _create_vbar_and_buttons(self, window_id, parent):
        """Create the widgets to display and command a window. These are:
            - Label with the window ID, on top.
            - Vertical bar to represent the window position (can be clicked to set the position), on the left.
            - Buttons to fully open and fully close the window, on the right. """
        
        # Frame to hold the window ID label, the window position bar and the open/close buttons 
        frame = ttk.Frame(parent)
        label = ttk.Label(frame, text=window_id, anchor=tk.CENTER, font=("Arial", 16, "bold"))
        vbar = CustomVerticalBar(frame)
        vbar.bind("<Button-1>", lambda event: self._vbar_on_click(event, window_id))
        btns_frame = ttk.Frame(frame)
        btn_up = ttk.Button(btns_frame, text="^", command=lambda: self.window_close(window_id), width=2)
        btn_down = ttk.Button(btns_frame, text="v", command=lambda: self.window_open(window_id), width=2)
        btn_spacer = ttk.Frame(btns_frame)
        
        # Placement of buttons within their frame, and other widgets in the main window frame
        btn_up.pack()
        btn_spacer.pack(fill=tk.Y, expand=True)
        btn_down.pack()
        label.grid(row=0, column=0, columnspan=2, sticky=tk.EW)
        vbar.grid(row=1, column=0, sticky=tk.NSEW, padx=5)
        btns_frame.grid(row=1, column=1, sticky=tk.NSEW)
        
        self.window_widgets[window_id]["vbar"] = vbar
        return frame


    def _vbar_on_click(self, event, window_id):
        """Event handler to trigger a window command when clicking on the progress bar."""

        vbar = self.window_widgets[window_id]["vbar"]
        click_position = event.y
        new_value = 100 - int((click_position / vbar.height) * 100)
        self.window_set(window_id, new_value)