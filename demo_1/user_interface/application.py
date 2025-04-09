"""Main application for the Cockpit and MobileApp in the window control example system.

This module provides the MainApp class, which is the main application
class for both the Cockpit and MobileApp applications.

The MainApp class ties together the CockpitConnext and GUI classes,
and provides the main application logic for these applications.

The MainApp class is responsible for creating the CockpitConnext and the appropriate
GUI instance (CockpitGUI or MobileAppGUI), and for setting up the communication between them.

The MainApp class is the entry point for the applications, and is
responsible for starting the application and cleaning up when the application
is closed.
"""

import argparse

import connext
import gui



class MainApp:
    """Main application class for the graphical applications."""

    def __init__(self, gui_app_class, domain_id=0):
        """Set up the CockpitConnext and CockpitGUI instances, 
        connecting their APIs and then start the application."""

        self.gui_app = gui_app_class()
        self.connext_app = connext.ConnextApp(domain_id)

        self.gui_app.window_open = self.connext_app.send_open
        self.gui_app.window_close = self.connext_app.send_close
        self.gui_app.window_set = self.connext_app.send_target

        self.connext_app.act_on_samples = lambda samples: [self.gui_app.update_position(sample.id, sample.position) for sample in samples]

        self.gui_app.start()

        self.connext_app.close()



if __name__ == "__main__":
    # Start the main application when executing this module
    parser = argparse.ArgumentParser(description="Start the Cockpit application.")
    parser.add_argument("--mode", choices=["cockpit", "mobileapp"], required=True, help="Mode to run the application in.")
    parser.add_argument("--domain_id", type=int, default=0, help="Domain ID for the Connext application.")
    args = parser.parse_args()

    if args.mode == "cockpit":
        app_class = gui.CockpitGUI
    elif args.mode == "mobileapp":
        app_class = gui.MobileAppGUI
    else:
        raise ValueError("Invalid mode selected.")
    
    app = MainApp(app_class, args.domain_id)