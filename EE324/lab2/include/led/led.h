#ifndef LED_H
#define LED_H

#include <xparameters.h>
#include <stdint.h>

#define LED_BASEADDR XPAR_MYLED_0_S_AXI_BASEADDR
#define LED_EN       *((uint32_t *) LED_BASEADDR)
#define LED_CTRL     *((uint32_t *) (LED_BASEADDR + 0x4))

uint32_t read_leds();

uint32_t write_leds(uint32_t values);

uint32_t write_led(int n, int value);

int toggle_led(int n);

int enable_led(int n);

int disable_led(int n);

#endif
