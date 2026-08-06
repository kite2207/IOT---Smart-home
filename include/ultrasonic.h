#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

void setup_ultrasonic(int trig_pin, int echo_pin);
String get_ultrasonic_status(int trig_pin, int echo_pin);

#endif