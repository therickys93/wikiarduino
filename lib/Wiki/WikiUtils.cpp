#include "Arduino.h"
#include "WikiUtils.h"
#include "bits.h"

#define ADDRESS_SHIFT     5U
#define PROTECTION_MASK 0xFC

#define CLOCK_BIT   BIT_3
#define DATA_BIT    BIT_4
#define ADDRESS_1   BIT_5
#define ADDRESS_2   BIT_6
#define ADDRESS_3   BIT_7

#define SET_BIT(REG, BIT)   ((REG) |=  (BIT))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))

int switch_on_led(int address)
{
    // PORTD
    uint8_t value = 0;
    // clear PORTD
    CLEAR_BIT(PORTD, ADDRESS_1);
    CLEAR_BIT(PORTD, ADDRESS_2);
    CLEAR_BIT(PORTD, ADDRESS_3);
    CLEAR_BIT(PORTD, DATA_BIT);
    CLEAR_BIT(PORTD, CLOCK_BIT);
    // set address
    value |= address << ADDRESS_SHIFT;
    PORTD |= (value & PROTECTION_MASK);
    // set data bit
    SET_BIT(PORTD, DATA_BIT);
    // set clock
    SET_BIT(PORTD, CLOCK_BIT);
    // clear clock
    CLEAR_BIT(PORTD, CLOCK_BIT);
    // clear data bit
    CLEAR_BIT(PORTD, DATA_BIT);
    // clear address
    CLEAR_BIT(PORTD, ADDRESS_1);
    CLEAR_BIT(PORTD, ADDRESS_2);
    CLEAR_BIT(PORTD, ADDRESS_3);
    return 0;
}

int switch_off_led(int address)
{
    // PORTD
    uint8_t value = 0;
    // clear PORTD
    CLEAR_BIT(PORTD, ADDRESS_1);
    CLEAR_BIT(PORTD, ADDRESS_2);
    CLEAR_BIT(PORTD, ADDRESS_3);
    CLEAR_BIT(PORTD, DATA_BIT);
    CLEAR_BIT(PORTD, CLOCK_BIT);
    // set address
    value |= address << ADDRESS_SHIFT;
    PORTD |= (value & PROTECTION_MASK);
    // set data bit
    CLEAR_BIT(PORTD, DATA_BIT);
    // set clock
    SET_BIT(PORTD, CLOCK_BIT);
    // clear clock
    CLEAR_BIT(PORTD, CLOCK_BIT);
    // clear data bit
    CLEAR_BIT(PORTD, DATA_BIT);
    // clear address
    CLEAR_BIT(PORTD, ADDRESS_1);
    CLEAR_BIT(PORTD, ADDRESS_2);
    CLEAR_BIT(PORTD, ADDRESS_3);
    return 0;
}