#include <stdio.h>
#include <unistd.h>
#include <string.h>

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
    //uint32_t bytes = 0x40000000;
    //tm1637_send_bytes((uint8_t*)&bytes, 1);
    //bytes = 0xc0ffffff;
    //tm1637_send_bytes((uint8_t*)&bytes, 4);

    uint8_t write_cmd = 0x40;
    //uint32_t nums = 0x01;
    uint8_t others = 0x01;
    uint8_t bytes[7];
	memset(bytes, 0, 7);
    bytes[0] = 0xc0;
    while (1)
    {
        //nums <<= 1;
        //if (!nums)
        //    nums = 0x01;
        //memcpy(bytes+1, &nums, 4);

        others <<= 1;
        if (!others)
            others = 0x01;
        bytes[1] = others;
        bytes[2] = others;
        bytes[3] = others;
        bytes[4] = others;
        bytes[5] = others;
		bytes[6] = others;

        //printf("printing %lu, %u\n", (unsigned long)nums, (unsigned char)others);
		printf("displaying %u\n", (unsigned char)others);
		tm1637_send_bytes(&write_cmd, 1);
        tm1637_send_bytes((uint8_t*)bytes, 7);
        sleep(2);
    }

    return 0;
}
