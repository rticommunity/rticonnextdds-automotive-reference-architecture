import connext
import gui


class CockpitApp:

    def __init__(self):
        self.connext_app = connext.CockpitConnext()
        self.gui_app = gui.CockpitGUI()

        self.gui_app.window_open = self.connext_app.send_open
        self.gui_app.window_close = self.connext_app.send_close
        self.gui_app.window_set = self.connext_app.send_target

        self.connext_app.act_on_samples = lambda samples: [self.gui_app.update_position(sample.id, sample.position) for sample in samples]

        self.gui_app.start()

        self.connext_app.close()
    

    def _act_on_samples_adaptor(self, samples):
        for sample in samples:
            self.gui_app.update_position(sample.id, sample.position)




if __name__ == "__main__":

    app = CockpitApp()