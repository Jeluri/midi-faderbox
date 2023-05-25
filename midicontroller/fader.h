#ifndef FADER_H
#define FADER_H

#include "control.h"

namespace mc {
    class Fader {
        private:
            int m_sliderPin;
            int m_pwmPin;
            int m_enablePin1;
            int m_enablePin2;
            int m_lowestPosition;
            int m_highestPosition;
            int m_comeAlive;
            bool m_isControlled;
            bool m_hasBeenPushed;
            float m_rawSetpoint;
            float m_setpoint;
            float m_position;
            float m_lastControlledPosition;
            float m_alphaPosition;
            float m_alphaSetpoint;
            PID m_controller;
            
            void driveMotor(float command);
        
        public:
            Fader(int sliderPin, int pwmPin, int enablePin1, int enablePin2);
            void setup();
            void attachController(PID controller);
            void setBounds(int lowestPosition, int highestPosition);
            void setComeAlive(int value);
            void smoothPosition(float alpha);
            void smoothSetpoint(float alpha);
            void update(bool debug = false);
            int getLevel();
            void moveTo(int level);
            void free();
            bool hasBeenPushed();
            void test();
    };
}

#endif
