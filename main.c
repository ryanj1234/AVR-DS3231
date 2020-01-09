#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "i2c.h"
#include "../ssd1306/ssd1306.h"

#define DS3231_I2C_ADDR     0xD0
#define I2C_WRITE_BIT       0x00
#define I2C_READ_BIT        0x01

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

static __inline__ void _read_byte(uint8_t reg, uint8_t *data)
{
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_WRITE_BIT);
    i2c_write(reg);
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_READ_BIT);
    *data = i2c_read_w_nack();
    i2c_stop();
}

static __inline__ void _read_word(uint8_t reg, uint16_t *data)
{
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_WRITE_BIT);
    i2c_write(reg);
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_READ_BIT);
    uint8_t lsb = 0, msb = 0;
    msb = i2c_read_w_ack();
    lsb = i2c_read_w_nack();
    i2c_stop();

    *data = ((uint16_t)msb << 8) | lsb;
}

static __inline__ void _write_byte(uint8_t reg, uint8_t data)
{
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_WRITE_BIT);
    i2c_write(reg);
    i2c_write(data);
    i2c_stop();
}

static __inline__ void _write_bytes(uint8_t reg, uint8_t *data, uint8_t n_bytes)
{
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_WRITE_BIT);
    i2c_write(reg);
    for(uint8_t i = 0; i < n_bytes; i++)
    {
        i2c_write(data[i]);
    }
    i2c_stop();
}

static void ds3231_read_register(uint8_t reg, uint8_t *data)
{
    _read_byte(reg, data);
}

static void ds3231_read_word(uint8_t reg, uint16_t *data)
{
    _read_word(reg, data);
}

static void ds3231_write_register(uint8_t reg, uint8_t data)
{
    _write_byte(reg, data);
}

static void ds3231_write_bytes(uint8_t reg, uint8_t *data, uint8_t n_bytes)
{
    _write_bytes(reg, data, n_bytes);
}

static void ds3231_read_time()
{
    uint8_t seconds = 0, minutes = 0, hour = 0, day = 0, month = 0, year = 0;

    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_WRITE_BIT);
    i2c_write(DS3231_SECONDS_REG);
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_READ_BIT);
    
    seconds = i2c_read_w_ack();
    minutes = i2c_read_w_ack();
    hour = i2c_read_w_nack();
    // day = i2c_read_w_ack();
    // month = i2c_read_w_ack();
    // year = i2c_read_w_nack();
    i2c_stop();

    ssd1306_putc('0' + ((hour >> 4) & 0x01));
    ssd1306_putc('0' + (hour & 0x0F));
    ssd1306_putc(':');
    ssd1306_putc('0' + ((minutes >> 4) & 0x0F));
    ssd1306_putc('0' + (minutes & 0x0F));
    ssd1306_putc(':');
    ssd1306_putc('0' + ((seconds >> 4) & 0x0F));
    ssd1306_putc('0' + (seconds & 0x0F));
    ssd1306_set_cursor(0, 1);
}

void ds3231_set_time(uint8_t hr, uint8_t min, uint8_t sec)
{
    i2c_start();
    i2c_write(DS3231_I2C_ADDR | I2C_WRITE_BIT);
    i2c_write(DS3231_SECONDS_REG);
    i2c_write(sec);
    i2c_write(min);
    i2c_write(hr);
    i2c_stop();
}

int main(void)
{
    i2c_init();
    ssd1306_init();
    ssd1306_clear_screen();
    ssd1306_puts("The time is: \n");
    // ds3231_set_time(TO_BCD(10), TO_BCD(44), TO_BCD(0));

    while(1) 
    {
        ds3231_read_time();
        _delay_ms(100);
    }
}