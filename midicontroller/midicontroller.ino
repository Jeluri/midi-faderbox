#include <Arduino.h>
#include <Bounce2.h>

#include "control.h"
#include "display.h"
#include "fader.h"
#include "midiccout.h"
#include "touchsense.h"
#include "utils.h"


// Boucle
#define LOOP_PERIOD_S       0.002

// Pins
#define PIN_PB1             0
#define PIN_PB2             1
#define PIN_D0              23
#define PIN_D1              22
#define PIN_D2              19
#define PIN_D3              20
#define PIN_D4              21  // Pin 20 mA
#define PIN_W1              17
#define PIN_W2              16
#define PIN_W3              15
#define PIN_W4              14
#define PIN_TS              18
#define PIN_PWM1            4
#define PIN_PWM2            6
#define PIN_PWM3            9
#define PIN_PWM4            10
#define PIN_E11             7
#define PIN_E12             5
#define PIN_E21             11
#define PIN_E22             8
#define PIN_E31             3
#define PIN_E32             2
#define PIN_E41             13
#define PIN_E42             12

// PWM
#define PWM_FTMO_1ST_PIN    6
#define PWM_FTM2_1ST_PIN    3
#define PWM_FREQ_HZ         234375
#define PWM_RESOLUTION      11

// Boutons
#define BTN_DEBOUNCE_MS     10
#define BTN_COOLDOWN_MS     250

// Faders
#define FDR_COUNT           4
#define FDR_INIT_MS         0
#define FDR_POS_MIN         25
#define FDR_POS_MAX         995
#define FDR_COME_ALIVE      150
#define FDR_POS_FILTER      0.2
#define FDR_SP_FILTER       0.02
#define FDR_PID_KP          16.0
#define FDR_PID_KI          7.0
#define FDR_PID_KD          0.2
#define FDR_U_MAX           2047
#define FDR_PID_ISAT        FDR_U_MAX
#define FDR_TS_THRES        6000
#define FDR_TS_COOLDOWN_MS  100

// MIDI
#define MIDI_CC_NUM_1       1   // Mod wheel
#define MIDI_CC_NUM_2       11  // Expression
#define MIDI_CC_NUM_3       16  // Generic
#define MIDI_CC_NUM_4       17  // Generic
#define MIDI_CC_HYST        2

// Liaison série
#define SRL_SPEED           57600


Button leftButton = Button();
Button rightButton = Button();

mc::Display display(PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4);
mc::TouchSense touchSensor(PIN_TS, FDR_TS_THRES);

mc::Fader faders[FDR_COUNT] = {
    mc::Fader(PIN_W1, PIN_PWM1, PIN_E11, PIN_E12),
    mc::Fader(PIN_W2, PIN_PWM2, PIN_E21, PIN_E22),
    mc::Fader(PIN_W3, PIN_PWM3, PIN_E31, PIN_E32),
    mc::Fader(PIN_W4, PIN_PWM4, PIN_E41, PIN_E42),
};

mc::MidiCcOut ccOut[FDR_COUNT] = {
    mc::MidiCcOut(MIDI_CC_NUM_1, MIDI_CC_HYST),
    mc::MidiCcOut(MIDI_CC_NUM_2, MIDI_CC_HYST),
    mc::MidiCcOut(MIDI_CC_NUM_3, MIDI_CC_HYST),
    mc::MidiCcOut(MIDI_CC_NUM_4, MIDI_CC_HYST)
};

int ccIn[FDR_COUNT] = {0};


void setup() {
    display.setup();
    display.print(1);

    leftButton.attach(PIN_PB1, INPUT_PULLUP);
    leftButton.setPressedState(LOW);
    leftButton.interval(BTN_DEBOUNCE_MS);

    rightButton.attach(PIN_PB2, INPUT_PULLUP);
    rightButton.setPressedState(LOW);
    rightButton.interval(BTN_DEBOUNCE_MS);

    analogWriteFrequency(PWM_FTMO_1ST_PIN, PWM_FREQ_HZ);  // Pour la PWM des faders n° 2 à 4
    analogWriteFrequency(PWM_FTM2_1ST_PIN, PWM_FREQ_HZ);  // Pour la PWM du fader n° 1
    analogWriteResolution(PWM_RESOLUTION);

    for (int i = 0; i < FDR_COUNT; i++) {
        faders[i].setup();
        
        faders[i].attachController(mc::PID(LOOP_PERIOD_S, FDR_PID_KP, FDR_PID_KI, FDR_PID_KD, FDR_PID_ISAT, FDR_U_MAX));
        
        faders[i].setBounds(FDR_POS_MIN, FDR_POS_MAX);
        faders[i].setComeAlive(FDR_COME_ALIVE);
        faders[i].smoothPosition(FDR_POS_FILTER);
        faders[i].smoothSetpoint(FDR_SP_FILTER);
    }
    
    touchSensor.setReleaseCooldown(FDR_TS_COOLDOWN_MS);

    usbMIDI.setHandleControlChange(onControlChange);

    Serial.begin(SRL_SPEED);
}


void loop() {
    elapsedMicros sinceLoopStart;
    
    static elapsedMicros sinceLastLoop;
    static elapsedMillis sinceButtonPressed;
    static int channel = 1;

    sinceLastLoop = 0;
    
    bool buttonCooldown = sinceButtonPressed < BTN_COOLDOWN_MS;
    bool channelChanged = false;
    
    leftButton.update();
    rightButton.update();
    touchSensor.update();
    usbMIDI.read();

    if (leftButton.pressed() or (leftButton.isPressed() and !buttonCooldown)) {
        if (--channel < 1) {
            channel = 16;
        }

        channelChanged = true;
    }
    else if (rightButton.pressed() or (rightButton.isPressed() and !buttonCooldown)) {
        if (++channel > 16) {
            channel = 1;
        }

        channelChanged = true;
    }

    if (channelChanged) {
        sinceButtonPressed = 0;
        display.print(channel);

        for (int i = 0; i < FDR_COUNT; i++) {
            ccOut[i].send(channel, true);
        }
    }
    
    for (int i = 0; i < FDR_COUNT; i++) {
        if (touchSensor.isTouched()) {
            faders[i].free();
        }
        else {
            faders[i].moveTo(ccIn[i]);
            // faders[i].test();
        }

        faders[i].update();
        ccOut[i].update(faders[i].getLevel());

        if (faders[i].hasBeenPushed()) {
            ccOut[i].send(channel);
        }
    }

    // Serial.println(sinceLoopStart);
    delayMicroseconds(LOOP_PERIOD_S * 1e6 - sinceLoopStart - 36);
}


void onControlChange(byte channel, byte control, byte value) {
    ccIn[control] = value;
}
