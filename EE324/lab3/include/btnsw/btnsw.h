#ifndef DRIVERS_BTNSW_H
#define DRIVERS_BTNSW_H

#include <xparameters.h>
#include <stdint.h>

#define BTNSW_BASEADDR XPAR_PROJ3_MYSWITCHES_BUT_0_S00_AXI_BASEADDR

uint8_t get_buttons() {
    return *((uint32_t *) BTNSW_BASEADDR + 1) & 0b1111;
}

uint16_t get_switches() {
    return *((uint32_t *) BTNSW_BASEADDR) & 0b11111111;
}

#endif
