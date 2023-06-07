#include "./led.h"

uint32_t read_leds() {
    return LED_CTRL & 0xF;
}

uint32_t write_leds(uint32_t values) {
    LED_CTRL = values & 0xF;

    return values;
}

uint32_t write_led(int n, int value) {
    uint32_t values = read_leds();
    values |= (1 << n);

    return write_leds(values);
}

int toggle_led(int n) {
    if (n > 4 || n < 0)
        return -1;
    uint32_t values = read_leds() ^ (1 << n);

    return write_leds(values);
}

int enable_led(int n) {
    LED_EN |= (1 << n);
}

int disable_led(int n) {
    LED_EN &= ~(1 << n);
}
