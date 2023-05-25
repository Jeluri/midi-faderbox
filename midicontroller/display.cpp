#include <Arduino.h>
#include "display.h"

using namespace mc;


Display::Display(int pin0, int pin1, int pin2, int pin3, int pin4) {
    m_controlPins[0] = pin0;
    m_controlPins[1] = pin1;
    m_controlPins[2] = pin2;
    m_controlPins[3] = pin3;
    m_controlPins[4] = pin4;
}

void Display::setup() {
    for (int i = 0 ; i < 5 ; i++) {
        pinMode(m_controlPins[i], OUTPUT);
    }
    
    print(0);
}

void Display::print(int number) {
    if (number < 0 or number > 19) {
        for (int i = 0 ; i < 4 ; i++) {
            digitalWrite(m_controlPins[i], HIGH);
        }
        
        digitalWrite(m_controlPins[4], LOW);
        
        return;
    }
    
    int lowerDigit = number % 10;
    int upperDigit = number >= 10;

    for (int i = 0 ; i < 4 ; i++) {
        digitalWrite(m_controlPins[i], lowerDigit & (1 << i));
    }
    
    digitalWrite(m_controlPins[4], upperDigit);
}
