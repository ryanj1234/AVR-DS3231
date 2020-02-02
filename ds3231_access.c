#include <avr/io.h>
#include "ds3231_access.h"
#include "i2cmaster.h"

#define DS3231_I2C_ADDR     0xD0

/** ds3231 i2c access functions */
void ds3231_read_register(uint8_t reg, uint8_t *data)
{
    i2c_start(DS3231_I2C_ADDR+I2C_WRITE);
    i2c_write(reg);
    i2c_start(DS3231_I2C_ADDR+I2C_READ);
    *data = i2c_readNak();
    i2c_stop();
}

void ds3231_read_word(uint8_t reg, uint16_t *data)
{
    i2c_start(DS3231_I2C_ADDR+I2C_WRITE);
    i2c_write(reg);
    i2c_start(DS3231_I2C_ADDR+I2C_READ);
    uint8_t lsb = 0, msb = 0;
    msb = i2c_readAck();
    lsb = i2c_readNak();
    i2c_stop();
    *data = ((uint16_t)msb << 8) | lsb;
}

void ds3231_write_register(uint8_t reg, uint8_t data)
{
    i2c_start(DS3231_I2C_ADDR+I2C_WRITE);
    i2c_write(reg);
    i2c_write(data);
    i2c_stop();
}

void ds3231_write_bytes(uint8_t reg, uint8_t *data, uint8_t n_bytes)
{
    i2c_start(DS3231_I2C_ADDR+I2C_WRITE);
    i2c_write(reg);
    for(uint8_t i = 0; i < n_bytes; i++)
    {
        i2c_write(data[i]);
    }
    i2c_stop();
}

void ds3231_read_3(uint8_t reg, uint8_t* dat1, uint8_t* dat2, uint8_t* dat3)
{
    i2c_start(DS3231_I2C_ADDR+I2C_WRITE);
    i2c_write(reg);
    i2c_start(DS3231_I2C_ADDR+I2C_READ);
    *dat1 = i2c_readAck();
    *dat2 = i2c_readAck();
    *dat3 = i2c_readNak();
    i2c_stop();
}

void ds3231_write_3(uint8_t reg, uint8_t dat1, uint8_t dat2, uint8_t dat3)
{
    i2c_start(DS3231_I2C_ADDR+I2C_WRITE);
    i2c_write(reg);
    i2c_write(dat1);
    i2c_write(dat2);
    i2c_write(dat3);
    i2c_stop();
}

void ds3231_read_bytes(uint8_t reg, uint8_t* buf, uint8_t cnt)
{
    i2c_start(DS3231_I2C_ADDR+I2C_WRITE);
    i2c_write(reg);
    i2c_start(DS3231_I2C_ADDR+I2C_READ);
    uint8_t i = 0;
    for(; i < (cnt-1); i++) { buf[i] = i2c_readAck(); }
    buf[i] = i2c_readNak();
    i2c_stop();
}