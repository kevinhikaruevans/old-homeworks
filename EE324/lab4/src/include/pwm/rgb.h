#ifndef PWM_RGB_H
#define PWM_RGB_H

#include <xparameters.h>
#include <stdbool.h>
#include <stdint.h>

#define RGB_BASEADDR XPAR_AXI_PWM_RGB_0_S00_AXI_BASEADDR

void set_rgb_pwm_freq(uint32_t freqHz) {
    uint32_t *p = (uint32_t *) PWM_BASEADDR;
    uint32_t div = 100e6 / freqHz;

    *p = (div << 16);
}

void set_rgb_pwm_enable(int num, bool en) {
    uint32_t *p = (uint32_t *) RGB_BASEADDR;
    *p ^= (en << (num - 1));
}

enum rgb_ch {
    CH_RED = 0, CH_GREEN = 1, CH_BLUE = 2
};

void set_rgb_pwm_value(int num, enum rgb_ch channel, uint16_t value) {
    uint32_t *p = (uint32_t *) RGB_BASEADDR + num;
    *p &= ~(0b111111111 << (channel * 10));
    *p |= (0b111111111 & value) << (channel * 10);
    //value << channel
}
#endif
