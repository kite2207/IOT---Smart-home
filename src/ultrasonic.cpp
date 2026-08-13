#include "ultrasonic.h"

void setup_ultrasonic(int trig_pin, int echo_pin)
{
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
    digitalWrite(trig_pin, LOW);
}

bool get_ultrasonic_status(int trig_pin, int echo_pin)
{
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    // Timeout 30ms (~5m); duration = 0 nghĩa là không đo được
    const unsigned long duration = pulseIn(echo_pin, HIGH, 30000);
    if (duration == 0) {
        return false;
    }

    const float distance_cm = duration / 58.0f;
    return distance_cm <= 50.0f;
}
