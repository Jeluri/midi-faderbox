#include <Arduino.h>
#include "fader.h"
#include "control.h"
#include "utils.h"

#define ANALOG_READ_MAX 1023
#define ANALOG_WRITE_MAX 65535 // Pour une résolution sur 16 bits

using namespace mc;

Fader::Fader(int sliderPin, int pwmPin, int enablePin1, int enablePin2):
    m_sliderPin(sliderPin),
    m_pwmPin(pwmPin),
    m_enablePin1(enablePin1),
    m_enablePin2(enablePin2),
    m_lowestPosition(0),
    m_highestPosition(ANALOG_READ_MAX),
    m_comeAlive(0.0),
    m_isControlled(false),
    m_hasBeenPushed(false),
    m_rawSetpoint(0.0),
    m_setpoint(0.0),
    m_position(0.0),
    m_lastControlledPosition(0.0),
    m_alphaPosition(1.0),
    m_alphaSetpoint(1.0),
    m_controller(PID(1.0, 0.0, 0.0, 0.0, 0.0, 0.0)) {
}

void Fader::setup() {
    pinMode(m_enablePin1, OUTPUT);
    pinMode(m_enablePin2, OUTPUT);

    driveMotor(0.0);

    m_position = analogRead(m_sliderPin);
    m_lastControlledPosition = m_position;
    m_rawSetpoint = m_position;
    m_setpoint = m_position;
}

void Fader::attachController(PID controller) {
    m_controller = controller;
}

void Fader::setBounds(int lowestPosition, int highestPosition) {
    m_lowestPosition = lowestPosition;
    m_highestPosition = highestPosition;
}

void Fader::setComeAlive(int value) {
    m_comeAlive = value;
}

void Fader::smoothPosition(float alpha) {
    m_alphaPosition = constrain(alpha, 0.0, 1.0);
}

void Fader::smoothSetpoint(float alpha) {
    m_alphaSetpoint = constrain(alpha, 0.0, 1.0);
}

void Fader::update(bool debug) {
    m_setpoint = m_alphaSetpoint * m_rawSetpoint + (1 - m_alphaSetpoint) * m_setpoint;
    
    float rawPosition = analogRead(m_sliderPin);
    m_position = m_alphaPosition * rawPosition + (1 - m_alphaPosition) * m_position;

    float command;
    if (m_isControlled) {
        command = m_controller.update(m_position, m_setpoint);
    }
    else {
        m_controller.reset();
        command = 0.0;

        if (abs(m_position - m_lastControlledPosition) > 10) {
            m_hasBeenPushed = true;
        }
    }

    driveMotor(command);

    if (debug) {
        Serial << m_rawSetpoint << "," << m_setpoint << "," << rawPosition << "," << m_position << "," << command/2 << "," << -1025 << "," << 1025 << '\n';
    }
}

int Fader::getLevel() {
    return constrain(round(127 * (m_position - m_lowestPosition) / (m_highestPosition - m_lowestPosition)), 0, 127);
}

void Fader::moveTo(int level) {
    m_rawSetpoint = constrain(level, 0, 127) / 127.0 * (m_highestPosition - m_lowestPosition) + m_lowestPosition;
    m_isControlled = true;
    m_hasBeenPushed = false;
}

void Fader::free() {
    if (m_isControlled) {
        m_lastControlledPosition = m_position;
    }

    m_rawSetpoint = m_position;
    m_setpoint = m_position;
    m_isControlled = false;
}

void Fader::driveMotor(float command) {
    if (!m_isControlled) {
        analogWrite(m_pwmPin, 0);
    }
    else if (abs(command) < m_comeAlive) {
        digitalWrite(m_enablePin1, LOW);
        digitalWrite(m_enablePin2, LOW);
        analogWrite(m_pwmPin, ANALOG_WRITE_MAX);
    }
    else {
        digitalWrite(m_enablePin1, command > 0);
        digitalWrite(m_enablePin2, command < 0);
        analogWrite(m_pwmPin, abs(command));
    }
}

bool Fader::hasBeenPushed() {
    return m_hasBeenPushed;
}

void Fader::test() {
    float period = 0.002;

    static elapsedMillis a;
    static int levelLow = random(6, 56);
    static int levelHigh = random(68, 118);
    static float increment = random(25, 250) * period;
    static float level = 0;

    if (a < 1000) {
        level = levelHigh;
    }
    else if (a < 2000) {
        level = levelLow;
    }
    else if (a < 5000) {
        level = min(level + increment, levelHigh);
    }
    else if (a < 8000) {
        level = max(level - increment, levelLow);
    }
    else {
        a = 0;
        levelLow = random(6, 56);
        levelHigh = random(68, 118);
        increment = random(25, 250) * period;
        level = levelHigh;
    }

    moveTo(level);
}
