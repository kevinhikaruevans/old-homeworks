#include "xil_types.h"

#include "led/led.h"
#include "rgbled/rgbled.h"

// Macro to access the base address of your LED controller
#define my_LED *((uint32_t *)0x4BB00000)

int main()
{
    int i = 0;

// * Requirement 1:
//    // Turn off all LEDs
//    my_LED = 0x00;
//
//    // Infinite loop
//    while (1) {
//
//        // invert the LED value
//        my_LED = ~my_LED;
//
//        // Delay
//        for (i = 0; i < 10000000; i++)
//            ;
//    }

// * Requirement 2:
//    my_LED = 0x1;
//
//    // Infinite loop
//    while (1) {
//
//        // invert the LED value
//        my_LED <<= 1;
//
//        if (my_LED == 0b10000) {
//            my_LED = 0x1;
//        }
//        // Delay
//        for (i = 0; i < 50000000; i++)
//            ;
//    }

// * Requirement 3:
//    int n = 0;
//    while (1) {
//        toggle_led(n);
//
//        if (++n == 4)
//            n = 0;
//
//        // Delay
//        for (i = 0; i < 50000000; i++)
//            ;
//    }

// * Requirement 4:
//    int n = 0;
//
//    // doing the same routine, but with only
//    // LEDs #0 and #1 enabled
//    enable_led(0);
//    enable_led(1);
//
//    while (1) {
//        toggle_led(n);
//
//        if (++n == 4)
//            n = 0;
//
//        // Delay
//        for (i = 0; i < 50000000; i++)
//            ;
//    }

// Challenge 1: RGB LED controller
    int n = 0;
    enable_rgb_led(0xFF);

    while (1) {
        set_rgb_led(0, n);
        set_rgb_led(1, n);

        if (++n == 0b1000) {
            n = 0;
        }

        // Delay
        for (i = 0; i < 50000000; i++)
            ;
    }
    return 1;
}
