#include "Arduino.h"
#include "Wiki.h"

int executeRequest(char *buffer)
{
    for(int index = 0; index < NUMBER_OF_LED; index++){
        char value = buffer[index];
        if(value == '0'){

        } else if(value == '1'){

        } else if(value == '2'){

        } else {
            return 1;
        }
    }
    return 0;
}