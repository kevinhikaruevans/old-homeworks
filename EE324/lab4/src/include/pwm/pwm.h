#ifndef PWM_H
#define PWM_H

#include <xparameters.h>
#include <stdint.h>
#include <stdbool.h>

#define PWM_MAXVAL 1023
#define PWM_BASEADDR XPAR_AXI_PWM4_0_S00_AXI_BASEADDR

void set_pwm_enable(int num, bool en) {
    uint32_t *p = (uint32_t *) PWM_BASEADDR;
    *p ^= (en << (num - 1));
}
void set_pwm_freq(int num, int freqHz) {
    uint32_t *p = (uint32_t *) PWM_BASEADDR;
    uint32_t div = 100e6 / freqHz;

    *(p + num) = (div << 16);
}

void set_pwm(int num, int val) {
    uint32_t *addr = (uint32_t *) PWM_BASEADDR;

    *(addr + num) &= ~(0b111111111);
    *(addr + num) |= val;
}

void set_pwm_float(int num, float value) {
    int ticks = value * PWM_MAXVAL;

    set_pwm(num, ticks);
}

#endif
