#include <stdio.h>
#include <errno.h>

#include "gpio.h"

#define GPIO_PREFIX         "/sys/class/gpio/"
#define GPIO_EXPORT_FN      GPIO_PREFIX "export"

#define GPIO_CLK_PIN        "66"    // P8, pin 7
#define GPIO_DAT_PIN        "67"    // P8, pin 8

#define GPIO_FOLDER(pin)    GPIO_PREFIX "gpio" pin "/"

#define GPIO_DIR_IN         "in"
#define GPIO_DIR_OUT        "out"
#define GPIO_DIR_FN(pin)    GPIO_FOLDER(pin) "direction"

#define GPIO_VAL_HIGH       "1"
#define GPIO_VAL_LOW        "0"
#define GPIO_VAL_FN(pin)    GPIO_FOLDER(pin) "value"

///
/// Helper functions
///
static int write_to_file(const char *fn, const char *str)
{
    FILE *f;
    int err;

    f = fopen(fn, "w");
    if (!f)
        return EIO;

    err = fputs(str, f) == EOF;
    fclose(f);

    return err;
}

///
/// Public functions
///
int gpio_init()
{
    int err;

    // export necessary pins
    err = write_to_file(GPIO_EXPORT_FN, GPIO_CLK_PIN);
    err |= write_to_file(GPIO_EXPORT_FN, GPIO_DAT_PIN);
    if (err)
    {
        printf("gpio_init: error occurred when exporting gpio pins\n");
        return err;
    }

    // set pins for output
    err = write_to_file(GPIO_DIR_FN(GPIO_CLK_PIN), GPIO_DIR_OUT);
    err |= write_to_file(GPIO_DIR_FN(GPIO_DAT_PIN), GPIO_DIR_OUT);
    if (err)
        printf("gpio_init: error occurred when setting gpio direction to out\n");

    return err;
}

int gpio_set_data_in()
{
    return write_to_file(GPIO_DIR_FN(GPIO_DAT_PIN), GPIO_DIR_IN);
}

int gpio_set_data_out()
{
    return write_to_file(GPIO_DIR_FN(GPIO_DAT_PIN), GPIO_DIR_OUT);
}

int gpio_clock_high()
{
    return write_to_file(GPIO_VAL_FN(GPIO_CLK_PIN), GPIO_VAL_HIGH);
}

int gpio_clock_low()
{
    return write_to_file(GPIO_VAL_FN(GPIO_CLK_PIN), GPIO_VAL_LOW);
}

int gpio_data_high()
{
    return write_to_file(GPIO_VAL_FN(GPIO_DAT_PIN), GPIO_VAL_HIGH);
}

int gpio_data_low()
{
    return write_to_file(GPIO_VAL_FN(GPIO_DAT_PIN), GPIO_VAL_LOW);
}

int gpio_get_data()
{
    FILE *f;
    char c;

    f = fopen(GPIO_VAL_FN(GPIO_DAT_PIN), "r");
    if (!f)
        return -EIO;

    c = fgetc(f);
    fclose(f);

    if (c == '0')
        return 0;
    else if (c == '1')
        return 1;
    else
        return -1;
}
