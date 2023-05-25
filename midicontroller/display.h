#ifndef DISPLAY_H
#define DISPLAY_H

namespace mc {
    class Display {
        private:
            int m_controlPins[5];
        
        public:
            Display(int pin0, int pin1, int pin2, int pin3, int pin4);
            void setup();
            void print(int number);
    };
}

#endif
