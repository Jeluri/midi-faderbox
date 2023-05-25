#include <Arduino.h>
#include "touchsense.h"

#define NSCAN 9
#define PRESCALE 2
#define REFERENCE_CURRENT 4
#define ELECTRODE_CURRENT 3

using namespace mc;

TouchSense::TouchSense(int pin, int threshold):
    m_pin(pin),
    m_threshold(threshold),
    m_releaseCooldown(0) {
    
    m_channel = pin2tsi[pin];
    
    scan();
}

void TouchSense::setReleaseCooldown(unsigned int value) {
    m_releaseCooldown = value;
}

void TouchSense::update() {
    if (!(TSI0_GENCS & TSI_GENCS_SCNIP)) {
        m_value = TSI0_DATA & 0xFFFF;
        
        scan();
    }
    
    if (isTouchedRaw()) {
        m_sinceReleased = 0;
    }
}

bool TouchSense::isTouchedRaw() {
    return m_value > m_threshold;
}

bool TouchSense::isTouched() {
    return isTouchedRaw() or m_sinceReleased < m_releaseCooldown;
}

void TouchSense::scan() {
    if (m_channel == 255) {
        return;
    }
    
    *portConfigRegister(m_pin) = PORT_PCR_MUX(0);
    
    SIM_SCGC5 |= SIM_SCGC5_TSI;
    TSI0_GENCS = TSI_GENCS_REFCHRG(REFERENCE_CURRENT) | TSI_GENCS_EXTCHRG(ELECTRODE_CURRENT) | TSI_GENCS_PS(PRESCALE)
                 | TSI_GENCS_NSCN(NSCAN) | TSI_GENCS_TSIEN;
    TSI0_DATA = TSI_DATA_TSICH(m_channel) | TSI_DATA_SWTS;
}
