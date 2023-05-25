#ifndef TOUCHSENSE_H
#define TOUCHSENSE_H

#include "touchsense.h"


namespace mc {
    const int pin2tsi[] = {
        //0    1    2    3    4    5    6    7    8    9
          9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
        255, 255,  14,  15, 255, 255, 255
    };
    
    class TouchSense {
        private:
            int m_pin;
            int m_channel;
            int m_threshold;
            unsigned int m_releaseCooldown;
            int m_value;
            elapsedMillis m_sinceReleased;
            
            void scan();
            bool isTouchedRaw();
        
        public:
            TouchSense(int pin, int threshold);
            void setReleaseCooldown(unsigned int value);
            void update();
            bool isTouched();
    };
}

#endif
