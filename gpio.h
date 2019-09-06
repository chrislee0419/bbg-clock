#ifndef _GPIO_H_
#define _GPIO_H_

int gpio_init();

int gpio_set_data_in();
int gpio_set_data_out();

int gpio_clock_high();
int gpio_clock_low();
int gpio_data_high();
int gpio_data_low();

int gpio_get_data();

#endif  // _GPIO_H_
