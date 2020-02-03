#ifndef __RTC_PRINT_H__
#define __RTC_PRINT_H__

#include "rtc.h"

#define RTC_FULL_PRINT  0

#if RTC_FULL_PRINT
void pretty_print(char * buf, RTC * time);
#endif
void compact_print(char * buf, RTC * time);

#endif