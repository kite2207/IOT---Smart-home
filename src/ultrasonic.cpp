#include "ultrasonic.h"

void setup_ultrasonic(int trig_pin, int echo_pin)
{
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
}

String get_ultrasonic_status(int trig_pin, int echo_pin)
{
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);

    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    
    digitalWrite(trig_pin, LOW);
    
    long duration = pulseIn(echo_pin, HIGH);
    long distance = duration / 58.2;

    if (distance > 0 && distance <= 50)
    {
        return "Có người";
    }

    return "Trống";
}