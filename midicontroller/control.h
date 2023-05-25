#ifndef CONTROL_H
#define CONTROL_H

namespace mc {
    class PID {
        private:
            float m_period;
            float m_Kp;
            float m_Ki;
            float m_Kd;
            float m_iSat;
            float m_uSat;
            float m_errorSum;
            float m_prevError;
            float m_output;

        public:
            PID(float period, float Kp, float Ki, float Kd, float iSat = 1e12, float uSat = 1e12);
            int getOutput();
            void reset();
            float update(float input, float setpoint);
    };
}

#endif
