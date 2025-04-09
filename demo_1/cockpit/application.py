"""Main application for the Cockpit app in the window control example system.

This module provides the CockpitApp class, which is the main application
class for the Cockpit application.

The CockpitApp class ties together the CockpitConnext and CockpitGUI classes,
and provides the main application logic for the Cockpit application.

The CockpitApp class is responsible for creating the CockpitConnext and CockpitGUI
instances, and for setting up the communication between them.

The CockpitApp class is the entry point for the Cockpit application, and is
responsible for starting the application and cleaning up when the application
is closed.
"""


import connext
import gui
import argparse

class CockpitApp:
    """Main application class for the Cockpit application."""

    def __init__(self, domain_id=0):
        """Set up the CockpitConnext and CockpitGUI instances, 
        connecting their APIs and then start the application."""

        self.gui_app = gui.CockpitGUI()
        self.connext_app = connext.CockpitConnext(domain_id=domain_id)

        self.gui_app.window_open = self.connext_app.send_open
        self.gui_app.window_close = self.connext_app.send_close
        self.gui_app.window_set = self.connext_app.send_target

        self.connext_app.act_on_samples = lambda samples: [self.gui_app.update_position(sample.id, sample.position) for sample in samples]

        self.gui_app.start()

        self.connext_app.close()



if __name__ == "__main__":
    # Start the Cockpit application when executing this module
    parser = argparse.ArgumentParser(description="Start the Cockpit application.")
    parser.add_argument("--domain_id", type=int, default=0, help="Domain ID for the Cockpit application")
    args = parser.parse_args()

    app = CockpitApp(domain_id=args.domain_id)