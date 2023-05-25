#ifndef MIDICCOUT_H
#define MIDICCOUT_H

namespace mc {
    class MidiCcOut {
        private:
            int m_number;
            int m_value;
            int m_hysteresis;
            int m_direction;
            bool m_hasChanged;

        public:
            MidiCcOut(int number, int hysteresis = 1);
            void update(float newValue);
            void send(int channel = 1, bool force = false);
    };
}

#endif
