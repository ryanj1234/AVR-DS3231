#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "rtc.h"
#include "rtc_print.h"

#if RTC_FULL_PRINT
void pretty_print(char * buf, RTC * time)
{
    static const char * months[12] = {
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

    static const char * wdays[7] = {
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Sat",
        "Sun"
    };
    sprintf(buf, 
            "%s, %s %d, %d %02d:%02d:%02d\n", 
            wdays[time->wday-1], 
            months[time->month-1], 
            time->mday, 
            time->year, 
            time->hour, 
            time->min, 
            time->sec);
}
#endif

void compact_print(char * buf, RTC * time)
{
    sprintf(buf, 
            "%u/%u/%u %02u:%02u:%02u\n", 
            time->year, 
            time->month, 
            time->mday, 
            time->hour, 
            time->min, 
            time->sec);
}