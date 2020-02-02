#include <avr/io.h>
#include "ds3231.h"
#include "ds3231_access.h"
#include "i2cmaster.h"
#include <stdio.h> // remove

/** Register defintions */
#define DS3231_SECONDS_REG          0x00
#define DS3231_MINUTES_REG          0x01
#define DS3231_HOUR_REG             0x02
#define DS3231_DAY_REG              0x03
#define DS3231_MONTH_CENT_REG       0x04
#define DS3231_DATE_REG             0x05
#define DS3231_YEAR_REG             0x06

/** Alarm 1 */
#define DS3231_ALARM1_SECONDS_REG       0x07
#define DS3231_ALARM1_MINUTES_REG       0x08
#define DS3231_ALARM1_HOUR_REG          0x09
#define DS3231_ALARM1_DAY_DATE_REG      0x0A

/** Alarm 2 */
#define DS3231_ALARM2_MINUTES_REG       0x0B
#define DS3231_ALARM2_HOUR_REG          0x0C
#define DS3231_ALARM2_DAY_DATE_REG      0x0D

#define DS3231_CONTROL_REG              0x0E
#define DS3231_CONTROL_STAT_REG         0x0F
#define DS3231_AGING_OFFSET_REG         0x10
#define DS3231_TEMP_MSB_REG             0x11
#define DS3231_TEMP_LSB_REG             0x12

#define TO_BCD(X)                       ((X/10) << 4) | ((X%10) & 0x0F)
#define NUM2ASCII(X)                    ('0' + X)

/** Time register masks */
#define DS3231_HOUR_10_MASK             0x10
#define DS3231_HOUR_20_MASK             0x20
#define DS3231_HOUR_AM_PM_MASK          0x20
#define DS3231_HOUR_12_24_MASK          0x40

/** Control bit definitions */
#define _A1IE       0
#define _A2IE       1
#define _INTCN      2
#define _RS1        3
#define _RS2        4
#define _CONV       5
#define _BBSQW      6
#define _EOSC       7

/** Control/Status bit definitions */
#define _A1F        0
#define _A2F        1
#define _BSY        2
#define _EN32K      3
#define _OSF        7

uint8_t _ctrl = 0x01C; // default value
uint8_t _ctrl_stat = 0x88; // default value
uint8_t _12_24_mode = 0; // 1 = 12hr mode

inline void _update_ctrl_reg(void)
{
    ds3231_write_register(DS3231_CONTROL_REG, _ctrl);
}

inline void ds3231_set_12hr_mode() { _12_24_mode = 1; }
inline void ds3231_set_24hr_mode() { _12_24_mode = 0; }

inline void ds3231_disable_32k()
{
    _ctrl_stat &= ~_BV(_EN32K);
    ds3231_write_register(DS3231_CONTROL_STAT_REG, _ctrl_stat);
}

inline void ds3231_enable_32k()
{
    _ctrl_stat |= _BV(_EN32K);
    ds3231_write_register(DS3231_CONTROL_STAT_REG, _ctrl_stat);
}

inline void ds3231_disable_alarm1()
{
    _ctrl &= ~_BV(_A1IE);
    _update_ctrl_reg();
}

inline void ds3231_enable_alarm1()
{
    _ctrl |= _BV(_A1IE);
    _update_ctrl_reg();
}

inline void ds3231_disable_alarm2()
{
    _ctrl &= ~_BV(_A1IE);
    _update_ctrl_reg();
}

inline void ds3231_enable_alarm2()
{
    _ctrl |= _BV(_A1IE);
    _update_ctrl_reg();
}

inline void ds3231_interrupt_disable()
{
    _ctrl &= ~_BV(_INTCN);
    _update_ctrl_reg();
}

inline void ds3231_interrupt_enable()
{
    _ctrl |= _BV(_INTCN);
    _update_ctrl_reg();
}

void ds3231_init()
{
    i2c_init();
}

void ds3231_read_time(uint8_t* hr, uint8_t* min, uint8_t* sec)
{
    ds3231_read_3(DS3231_SECONDS_REG, sec, min, hr);
}

void ds3231_set_time(uint8_t hr, uint8_t min, uint8_t sec)
{
    hr = _12_24_mode ? hr | DS3231_HOUR_12_24_MASK : hr;
    ds3231_write_3(DS3231_SECONDS_REG, sec, min, hr);
}

static inline void _print_12hr_hr(char* buf, uint8_t hr)
{
    buf[0] = NUM2ASCII(((hr >> 4) & 0x01));
    buf[1] = NUM2ASCII((hr & 0x0F));
    buf[2] = ':';
}

static inline void _print_min_sec(char* buf, uint8_t min, uint8_t sec)
{
    buf[3] = NUM2ASCII(((min >> 4) & 0x0F));
    buf[4] = NUM2ASCII((min & 0x0F));
    buf[5] = ':';

    buf[6] = NUM2ASCII(((sec >> 4) & 0x0F));
    buf[7] = NUM2ASCII((sec & 0x0F));
}

static inline void _print_am_pm(char* buf, uint8_t hr)
{
    buf[9] = (hr & DS3231_HOUR_AM_PM_MASK) ? 'p' : 'a';

    buf[10] = 'm';
    buf[11] = 0;
}

void print_12hr_time(char* buf, uint8_t hr, uint8_t min, uint8_t sec)
{
    _print_12hr_hr(buf, hr);
    _print_min_sec(buf, min, sec);
    buf[8] = ' ';
    _print_am_pm(buf, hr);
}

/**
 * RTC initialization
 * 
 * @return  1 if success, else 0
 */
int rtc_init()
{
    i2c_init();
    return 1;
}

static inline uint8_t hour_decode(uint8_t raw)
{
    uint8_t hour = raw & 0x0F;

    if(raw & 0x10) { hour += 10; }

    if(raw & DS3231_HOUR_12_24_MASK)
    {
        _12_24_mode = 1;
        if(raw & 0x20) { hour += 12; }
    }
    else
    {
        _12_24_mode = 0;
        if(raw & 0x20) { hour += 20; }
    }

    return hour;
}

/**
 * Read current time from RTC
 * 
 * @return  1 if success, else 0
 */
int rtc_gettime(RTC* rtc)
{
    uint8_t buf[7];
    ds3231_read_bytes(0x00, buf, 7);
    rtc->sec = (buf[0] & 0x0F) + 10*(buf[0] >> 4);
    rtc->min = (buf[1] & 0x0F) + 10*(buf[1] >> 4);
    rtc->hour = hour_decode(buf[2]);
    rtc->wday = buf[3];
    rtc->mday = (buf[4] & 0x0F) + 10*(buf[4] >> 4);
    rtc->month = (buf[5] & 0x0F) + 10*((buf[5] >> 4) & 0x01);
    rtc->year = 1900 + (buf[6] & 0x0F) + 10*(buf[6] >> 4);
    if(buf[5] & 0x80) { rtc->year += 100; }
    return 1;
}

/**
 * Set time in the RTC
 * 
 * @return 1 if success, else 0
 */
int rtc_settime(const RTC* time)
{
    uint8_t buf[7];
    buf[0] = ((time->sec/10) << 4) | (time->sec % 10);
    buf[1] = ((time->min/10) << 4) | (time->min % 10);
    buf[2] = (time->hour % 10);
    if(time->hour > 19) { buf[2] |= 0x20; }
    else if(time->hour > 9) { buf[2] |= 0x10; }
    buf[3] = time->wday;
    buf[4] = ((time->mday/10) << 4) | (time->mday % 10);
    buf[5] = ((time->month/10) << 4) | (time->month % 10);
    uint8_t tmp_year;
    if(time->year > 1999)
    {
        buf[5] |= 0x80;
        tmp_year = time->year - 2000;
    }
    else
    {
        tmp_year = time->year - 1900;
    }
    buf[6] = ((tmp_year/10) << 4) | (tmp_year % 10);
    ds3231_write_bytes(0, buf, 7);
    return 1;
}