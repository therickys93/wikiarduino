#include "Arduino.h"
#include "Wiki.h"
#include "WikiUtils.h"

int executeRequest(char *buffer)
{
    for(int index = 0; index < NUMBER_OF_LED; index++){
        char value = buffer[index];
        if(value == '0'){
            switch_off_led(index);
        } else if(value == '1'){
            switch_on_led(index);
        } else if(value == '2'){
            switch_off_led(index);
            delay(100);
            switch_on_led(index);
            delay(100);
            switch_off_led(index);
        } else {
            return 1;
        }
    }
    return 0;
}