#ifndef RGBLED_H
#define RGBLED_H

#include <stdint.h>
#include <xparameters.h>

#define RGBLED_BASEADDR XPAR_DUAL_RGB_LED_0_S_AXI_BASEADDR
#define RGBLED_EN       *((uint32_t *) RGBLED_BASEADDR)
#define RGBLED_CTRL0    *((uint32_t *) (RGBLED_BASEADDR + 0x4))
#define RGBLED_CTRL1    *((uint32_t *) (RGBLED_BASEADDR + 0x8))

void enable_rgb_led(int n) {
    RGBLED_EN |= n;
}

void disable_rgb_led(int n) {
    RGBLED_EN &= ~(1 << n);
}

void set_rgb_led(int n, int value) {
    if (n == 0) {
        RGBLED_CTRL0 = value;
    } else {
        RGBLED_CTRL1 = value;
    }
}
#endif
