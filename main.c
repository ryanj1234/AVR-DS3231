#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "ds3231.h"
#include "uart.h"
#include "rtc.h"
#include "rtc_print.h"

void configure_int0(void)
{
    DDRD &= ~(1 << PIND2);
    PORTD |= (1 << PIND2);
    EICRA |= (1 << ISC01); // trigger on rising edge
    EIMSK |= (1 << INT0);  // enable int0 interrupts
}

volatile uint8_t update_time = 0;
ISR(INT0_vect)
{
    update_time = 1;
}

int main(void)
{
    uart_init(9600);
    
    ds3231_dev ds3231 = get_ds3213();

    /** alarm time */
    RTC atime;
    atime.sec = 0;
    ds3231_interrupt_enable(&ds3231);
    ds3231_enable_alarm1(&ds3231);

    ds3231_clear_alarm1_flag(&ds3231);
    configure_int0();
    sei();

    RTC rtc;
    char buf[32];
    while(1)
    {
        ds3231_gettime(ds3231, &rtc);
        compact_print(buf, &rtc);
        uart_puts(buf);

        ds3231_clear_alarm1_flag(&ds3231);
        atime.sec = rtc.sec + 5;
        if(atime.sec > 59) { atime.sec -= 60; }
        ds3231_set_alarm1(&ds3231, &atime, match_seconds);

        while(!update_time) {}
        update_time = 0;
    }
}