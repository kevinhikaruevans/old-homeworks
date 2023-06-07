#ifndef DRIVERS_LED_H
#define DRIVERS_LED_H

#include <xparameters.h>
#include <stdint.h>

#define LED_BASEADDR XPAR_PROJ3_AXI_IP_0_S00_AXI_BASEADDR

void set_leds(uint8_t value) {
    *((uint32_t *) LED_BASEADDR) = value;
}

void set_rgbleds(uint16_t value) {
    *((uint32_t *) LED_BASEADDR + 1) = value;
}
#endif
