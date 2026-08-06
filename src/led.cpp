#include "led.h"

#include <Arduino.h>

#include "config.h"

namespace {
bool currentState = false;
}

void setupLed() {
    pinMode(LED_PIN, OUTPUT);
    setLed(false);
}

void setLed(bool isOn) {
    currentState = isOn;
    digitalWrite(LED_PIN, isOn ? HIGH : LOW);
}

bool ledIsOn() {
    return currentState;
}
