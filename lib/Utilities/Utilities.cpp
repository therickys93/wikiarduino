#include "Arduino.h"
#include "Utilities.h"

void pinInput(int pin)
{
    pinMode(pin, INPUT);
}

void pinOutput(int pin)
{
    pinMode(pin, OUTPUT);
}

void pinHigh(int pin)
{
    digitalWrite(pin, HIGH);
}

void pinLow(int pin)
{
    digitalWrite(pin, LOW);
}

int pinState(int pin)
{
    return digitalRead(pin);
}