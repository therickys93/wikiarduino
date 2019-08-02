#ifndef _WIKI_H
#define _WIKI_H

#include "Arduino.h"
#include "pins.h"

#define NUMBER_OF_LED   8
#define LED_START_INDEX PIN_2
#define LED_STOP_INDEX  PIN_9

int executeRequest(char *buffer);

#endif