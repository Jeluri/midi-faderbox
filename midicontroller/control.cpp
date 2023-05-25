#include <Arduino.h>
#include "control.h"
#include "utils.h"

using namespace mc;


PID::PID(float period, float Kp, float Ki, float Kd, float iSat, float uSat):
    m_period(period),
    m_Kp(Kp),
    m_Ki(Ki),
    m_Kd(Kd),
    m_iSat(iSat),
    m_uSat(uSat),
    m_errorSum(0.0),
    m_prevError(0.0),
    m_output(0.0) {
}

int PID::getOutput() {
    return m_output;
}

void PID::reset() {
    m_prevError = 0.0;
    m_errorSum = 0.0;
}

float PID::update(float input, float setpoint) {
    float error = input - setpoint;
    m_errorSum = constrain(m_errorSum + error * m_period, -m_iSat, m_iSat);

    if (sign(error) != sign(m_prevError)) {
        reset();
    }

    m_output = m_Kp * error + m_Ki * m_errorSum + m_Kd * (error - m_prevError) / m_period;
    m_output = constrain(m_output, -m_uSat, m_uSat);
    m_prevError = error;
    
    return m_output;
}
