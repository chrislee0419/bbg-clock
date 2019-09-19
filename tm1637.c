#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "tm1637.h"
#include "gpio.h"

#define CLOCK_DURATION_NS       1e6		// 100 microseconds
static struct timespec clockDelay = { .tv_sec = 0, .tv_nsec = CLOCK_DURATION_NS };
#define CLOCK_SLEEP             nanosleep(&clockDelay, NULL)

#define MAX_CMD_LEN             8

#define CMD_WRITE_DATA          0x40

#define CMD_DISP_CONTROL        0x80
#define CMD_DISP_OFF            0
#define CMD_DISP_ON             0x08

#define CMD_SET_ADDR            0xc0
#define CMD_SET_ADDR_START      0

#define SEG_TOP                 0x01
#define SEG_TOP_R               0x02
#define SEG_BOT_R               0x04
#define SEG_BOT                 0x08
#define SEG_BOT_L               0x10
#define SEG_TOP_L               0x20
#define SEG_MID                 0x40
#define SEG_DECIMAL             0x80

static uint8_t CONVERSION_TABLE[10] =
{
    SEG_TOP | SEG_TOP_L | SEG_TOP_R | SEG_BOT | SEG_BOT_L | SEG_BOT_R,              // 0
    SEG_TOP_R | SEG_BOT_R,                                                          // 1
    SEG_TOP | SEG_TOP_R | SEG_MID | SEG_BOT_L | SEG_BOT,                            // 2
    SEG_TOP | SEG_TOP_R | SEG_MID | SEG_BOT_R | SEG_BOT,                            // 3
    SEG_TOP_L | SEG_MID | SEG_TOP_R | SEG_BOT_R,                                    // 4
    SEG_TOP | SEG_TOP_L | SEG_MID | SEG_BOT_R | SEG_BOT,                            // 5
    SEG_TOP | SEG_TOP_L | SEG_BOT_L | SEG_BOT | SEG_BOT_R | SEG_MID,                // 6
    SEG_TOP | SEG_TOP_R | SEG_BOT_R,                                                // 7
    SEG_TOP | SEG_TOP_L | SEG_TOP_R | SEG_MID | SEG_BOT_L | SEG_BOT_R | SEG_BOT,    // 8
    SEG_TOP | SEG_TOP_L | SEG_TOP_R | SEG_MID | SEG_BOT_R                           // 9
};

///
/// Helper functions
///
static void send_start()
{
    // "When CLK is a high level and DIO changes from high to low level, data input starts."
    gpio_data_high();
    gpio_clock_high();
    CLOCK_SLEEP;
    gpio_data_low();
}

static void send_stop()
{
    // "When CLK is a high level and DIO changes from low level to high level, data input ends."
    gpio_clock_low();
    CLOCK_SLEEP;
    gpio_data_low();
    CLOCK_SLEEP;
    gpio_clock_high();
    CLOCK_SLEEP;
    gpio_data_high();
}

static uint8_t handle_ack()
{
    // "For a right data transfer, an answering signal ACK is generated inside the chip to lower the DIO pin"
    gpio_clock_low();
    gpio_set_data_in();
    CLOCK_SLEEP;

    uint8_t ack = 0;
	ack = gpio_get_data();
    gpio_clock_high();
    CLOCK_SLEEP;
    gpio_clock_low();

    gpio_set_data_out();

    if (ack < 0)
        printf("tm1637:handle_ack: failed to get ack\n");

    return ack;
}

static void send_byte(uint8_t byte)
{
    // When data is input, (...) DIO signal should change for low level CLK signal.
    for (int i = 0; i < 8; ++i)
    {
        gpio_clock_low();

        if (byte & 1)
            gpio_data_high();
        else
            gpio_data_low();
        CLOCK_SLEEP;

        gpio_clock_high();
        CLOCK_SLEEP;

        byte >>= 1;
    }
}

static int send_command(uint8_t *cmd, size_t cmd_size)
{
    send_start();

    uint8_t ack;
    uint8_t hadRedo = 0;
    for (int i = 0; i < cmd_size; ++i)
    {
        send_byte(cmd[i]);

        if ((ack = handle_ack()) && !hadRedo)
        {
            printf("tm1637:send_command: ack failed, redoing\n");
            hadRedo = 1;
            --i;
        }
        else if (ack)
        {
            printf("tm1637:send_command: ack failed, skipping\n");
            hadRedo = 0;
        }
        else
        {
            hadRedo = 0;
        }
    }

    send_stop();

    return 0;
}

static void convert_number(uint8_t num, uint8_t *out1, uint8_t *out2, uint8_t showZeroInTens)
{
    if (num > 99)
    {
        printf("tm_1637:convert_number: unable to convert number larger than 100\n");
        return;
    }

    uint8_t tens = num / 10;
    uint8_t ones = num % 10;

    *out1 = tens == 0 && !showZeroInTens ? 0x0 : CONVERSION_TABLE[tens];
    *out2 = CONVERSION_TABLE[ones];
}

///
/// Public functions
///
int tm1637_init()
{
    int err = gpio_init();
    err |= gpio_clock_low();
    err |= gpio_data_low();
    if (err)
        printf("tm1637_init: failed\n");

    return err;
}

int tm1637_set_brightness(uint8_t brightness)
{
    uint8_t controlWord;

    if (brightness == 0)
    {
        controlWord = CMD_DISP_CONTROL | CMD_DISP_OFF;
    }
    else
    {
        if (brightness > 8)
            brightness = 8;

        controlWord = CMD_DISP_CONTROL | CMD_DISP_ON | (brightness - 1);
    }

    return send_command(&controlWord, 1);
}

int tm1637_set_time(uint8_t hour, uint8_t minute, uint8_t pm)
{
    uint8_t data[MAX_CMD_LEN];
    int err = 0;

    // set to write to memory mode with auto incrementing address
    data[0] = CMD_WRITE_DATA;
    err = send_command(data, 1);
    if (err)
    {
        printf("tm1637_set_time: error occurred during send write command\n");
        return err;
    }

    data[0] = CMD_SET_ADDR | CMD_SET_ADDR_START;

    if (hour > 12)
        hour = 12;
    convert_number(hour, data+1, data+2, 0);

    if (minute > 59)
        minute = 59;
    convert_number(minute, data+3, data+4, 1);

    // colon and decimals?
    data[5] = 0xff;

    err = send_command(data, 6);
    if (err)
        printf("tm1637_set_time: error occurred when setting digits\n");

    return err;
}

void tm1637_send_bytes(uint8_t *bytes, uint8_t size)
{
    send_command(bytes, size);
}
