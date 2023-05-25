#include <Arduino.h>
#include "midiccout.h"
#include "utils.h"

using namespace mc;

MidiCcOut::MidiCcOut(int number, int hysteresis):
    m_number(number),
    m_value(0),
    m_hysteresis(hysteresis),
    m_direction(0),
    m_hasChanged(false) {
}

void MidiCcOut::update(float newValue) {
    int delta = newValue - m_value;
    int direction = sign(delta);

    if (abs(delta) >= m_hysteresis or direction == m_direction) {
        m_value = newValue;
        m_direction = direction;
        m_hasChanged = true;
    }
}

void MidiCcOut::send(int channel, bool force) {
    if (m_hasChanged or force) {
        usbMIDI.sendControlChange(m_number, m_value, channel);
        m_hasChanged = false;
    }
}
