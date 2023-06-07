#include "pwm/pwm.h"
#include "pwm/rgb.h"
void wait() {
    for (int i = 0; i < 0x000FFFFF; i++)
        ;
}

int main() {
    set_rgb_pwm_freq(100e3);
    set_rgb_pwm_enable(1, 1);
    set_rgb_pwm_enable(2, 1);
    set_rgb_pwm_enable(3, 1);
    set_rgb_pwm_enable(4, 1);

    while (1) {
        for (int led = 1; led <= 4; led++) {
            for (int c = 0; c < 3; c++) {
                for (int i = 0; i < 20; i++) {
                    set_rgb_pwm_value(led, c, i);
                    wait();
                }
            }
        }
        for (int led = 1; led <= 4; led++) {
            for (int c = 0; c < 3; c++) {
                for (int i = 20; i >= 0; i--) {
                    set_rgb_pwm_value(led, c, i);
                    wait();
                }
            }
        }
    }
//    set_pwm_enable(1, 1);
//    set_pwm_enable(2, 1);
//    set_pwm_enable(3, 1);
//    set_pwm_enable(4, 1);
//
//    set_pwm_freq(1, 20e6); // 100 kHz
//    set_pwm_freq(2, 25e6); // 100 kHz
//    set_pwm_freq(3, 33e6);   // 1 MHz
//    set_pwm_freq(4, 50e6);   // 1 MHz
//
//    set_pwm(1, 1000);
//    set_pwm(2, 1000);
//    set_pwm(3, 1000);
//    set_pwm(4, 1000);

    while (1) {
        /*for (int i = 1; i <= 4; i++) {
         for (int j = 0; j < 1000; j++) {
         set_pwm(i, j);
         wait();
         }
         for (int j = 1000; j >= 0; j--) {
         set_pwm(i, j);
         wait();
         }
         }*/
    }

    return 0;
}
