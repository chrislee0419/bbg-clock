#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#include "tm1637.h"

int main()
{
    if (tm1637_init())
    {
        printf("error occurred during initialization, shutting down\n");
        return 1;
    }

    tm1637_set_brightness(8);

    time_t rawtime;
    struct tm *time_info;
    uint8_t current_hour;
    uint8_t current_minute;
    while (1)
    {
        // refresh time every day
        time(&rawtime);
        time_info = localtime(&rawtime);

        current_hour = time_info->tm_hour;
        current_minute = time_info->tm_min;

        printf("setting current time display to: %2u:%2u\n", current_hour, current_minute);
        fflush(stdout);

        tm1637_set_time(current_hour, current_minute, 1);

        sleep(60 - time_info->tm_sec);
        while (1)
        {
            // increment minute/hour
            ++current_minute;
            if (current_minute >= 60)
            {
                ++current_hour;
                current_minute = 0;
            }

            if (current_hour >= 24)
                break;

            tm1637_set_time(current_hour, current_minute, 1);
            sleep(60);
        }
    }

    return 0;
}
