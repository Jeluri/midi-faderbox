#name=Fader Box


import device
import midi


CC_NUMBERS = [1, 11, 16, 17]


class FaderBoxInterface:
    def __init__(self):
        self.device_name = ""
        self.port_number = 0
        self.channel_number = 1
        self.last_send_values = [None] * 4

    def on_init(self):
        self.device_name = device.getName()
        self.port_number = device.getPortNumber()

    def on_cc_in(self, event):
        # self.channel_number = event.midiChan + 1
        self.channel_number = (event.status & 0xF) + 1  # Contournement du bug où event.midiChan toujours nul

    def on_cc_out(self):
        for fader_id, cc_number in enumerate(CC_NUMBERS):
            control_id = cc_number + ((self.channel_number - 1) << 16) + ((self.port_number + 1) << 22)
            event_id = device.findEventID(control_id)

            if event_id != midi.REC_InvalidID:
                value = device.getLinkedValue(event_id)
                value_128 = round(value * 127)

                if value_128 != self.last_send_values[fader_id]:
                    device.midiOutMsg(midi.MIDI_CONTROLCHANGE, 0, fader_id, value_128)
                    self.last_send_values[fader_id] = value_128

                    name = device.getLinkedParamName(event_id)
                    print(f"{name} = {value_128} ({value:.1%})")


faderbox = FaderBoxInterface()


def OnInit():
    faderbox.on_init()

def OnChannelChange(event):
    faderbox.on_cc_in(event)

def OnRefresh(flag):
    if flag & midi.HW_Dirty_ControlValues:
        faderbox.on_cc_out()
