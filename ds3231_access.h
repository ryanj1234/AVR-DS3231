#ifndef __DS3231_ACCESS_h__
#define __DS3231_ACCESS_h__

#include <avr/io.h>

void ds3231_read_register(uint8_t reg, uint8_t *data);
void ds3231_read_word(uint8_t reg, uint16_t *data);
void ds3231_write_register(uint8_t reg, uint8_t data);
void ds3231_write_bytes(uint8_t reg, uint8_t *data, uint8_t n_bytes);
void ds3231_read_3(uint8_t reg, uint8_t* dat1, uint8_t* dat2, uint8_t* dat3);
void ds3231_write_3(uint8_t reg, uint8_t dat1, uint8_t dat2, uint8_t dat3);
void ds3231_read_bytes(uint8_t reg, uint8_t* buf, uint8_t cnt);

#endif