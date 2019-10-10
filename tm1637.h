#ifndef _TM1637_H_
#define _TM1637_H_

// adapted from: https://github.com/bitbank2/tm1637
// referenced from: https://www.mcielectronics.cl/website_MCI/static/documents/Datasheet_TM1637.pdf

#include <stdint.h>

int tm1637_init();

int tm1637_set_brightness(uint8_t brightness);
int tm1637_set_time(uint8_t hour, uint8_t minute, uint8_t colon);

void tm1637_send_bytes(uint8_t *bytes, uint8_t size);

#endif  // _TM1637_H_
