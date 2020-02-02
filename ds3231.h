#ifndef __DS3231_h__
#define __DS3231_h__

#include <avr/io.h>

typedef struct {
	uint16_t	year;	/* 2000..2099 */
	uint8_t		month;	/* 1..12 */
	uint8_t		mday;	/* 1.. 31 */
	uint8_t		wday;	/* 1..7 */
	uint8_t		hour;	/* 0..23 */
	uint8_t		min;	/* 0..59 */
	uint8_t		sec;	/* 0..59 */
} RTC;

int rtc_init(void);                 /* Initialize RTC */
int rtc_gettime(RTC*);				/* Get time */
int rtc_settime(const RTC*);		/* Set time */

void ds3231_init(void);

void ds3231_read_time(uint8_t* hr, uint8_t* min, uint8_t* sec);
void ds3231_set_time(uint8_t hr, uint8_t min, uint8_t sec);

void ds3231_set_12hr_mode(void);
void ds3231_set_24hr_mode(void);
void ds3231_disable_32k(void);
void ds3231_enable_32k(void);
void ds3231_disable_alarm1(void);
void ds3231_enable_alarm1(void);
void ds3231_disable_alarm2(void);
void ds3231_enable_alarm2(void);
void ds3231_interrupt_disable(void);
void ds3231_interrupt_enable(void);

void print_12hr_time(char* buf, uint8_t hr, uint8_t min, uint8_t sec);

#endif