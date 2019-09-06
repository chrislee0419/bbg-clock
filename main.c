#include <stdio.h>

#include "tm1637.h"

int main()
{
    if (tm1637_init())
    {
        printf("Error occurred during initialization, shutting down\n");
        return 1;
    }

    //tm1637_set_brightness(8);
    //tm1637_set_time(12, 59, 1);

    tm1637_set_brightness(4);
    uint32_t bytes = 0x40000000;
    tm1637_send_bytes((uint8_t*)&bytes, 1);
    bytes = 0xc0ffffff;
    tm1637_send_bytes((uint8_t*)&bytes, 4);

    return 0;
}
