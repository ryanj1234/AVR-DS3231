#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ds3231.h"
#include "ssd1306.h"
#include "uart.h"

enum weekday
{
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6,
    Sunday = 7
};

enum month
{
    January = 1,
    February = 2,
    March = 3,
    April = 4,
    May = 5,
    June = 6,
    July = 7,
    August = 8,
    September = 9,
    October = 10,
    November = 11,
    December = 12
};

void pretty_print(char * buf, RTC * time)
{
    static char* months[12] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };

    static char * wdays[7] = {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"
    };
    sprintf(buf, "%s, %s %d, %d %02d:%02d:%02d\n", wdays[time->wday-1], months[time->month-1], time->mday, time->year, time->hour, time->min, time->sec);
}

void compact_print(char * buf, RTC * time)
{
    sprintf(buf, "%u/%u/%u %02u:%02u:%02u\n", time->year, time->month, time->mday, time->hour, time->min, time->sec);
}

int main(void)
{
    ds3231_init();
    uart_init(9600);

    RTC rtc;
    // RTC stime;
    // stime.sec = 0;
    // stime.min = 5;
    // stime.hour = 18;
    // stime.mday = 1;
    // stime.wday = (uint8_t)saturday;
    // stime.month = (uint8_t)February;
    // stime.year = 2020;
    // rtc_settime(&stime);

    char buf[32];
    while(1)
    {
        rtc_gettime(&rtc);
        compact_print(buf, &rtc);
        uart_puts(buf);

        _delay_ms(250);
    }
}