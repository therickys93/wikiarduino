#include "Arduino.h"
#include "WikiUtils.h"
#include "bits.h"

#define ADDRESS_SHIFT 3U

#define SET_BIT(REG, BIT)   ((REG) |=  (BIT))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))

int switch_on_led(int address)
{
    // PORTD
    uint8_t value = 0;
    // set address
    value |= address << ADDRESS_SHIFT;
    // set data bit
    SET_BIT(value, BIT_2);
    // set clock
    SET_BIT(value, BIT_1);
    // clear clock
    CLEAR_BIT(value, BIT_1);
    return 0;
}

int switch_off_led(int address)
{
    // PORTD
    uint8_t value = 0;
    // set address
    value |= address << ADDRESS_SHIFT;
    // set data bit
    CLEAR_BIT(value, BIT_2);
    // set clock bit
    SET_BIT(value, BIT_1);
    // clear clock bit
    CLEAR_BIT(value, BIT_1);
    return 0;
}