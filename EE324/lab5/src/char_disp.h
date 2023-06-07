#ifndef CHAR_DISP_H
#define CHAR_DISP_H

#include <xparameters.h>
#include <stdint.h>

#define CHAR_DISP_BASEADDR XPAR_AXI_CHAR_DISP_0_S00_AXI_BASEADDR

//void disp_char(char val, uint32_t x, uint32_t y) {
//	uint32_t regval = (x << 22) | ((y & 0b1111111111) << 12) | val;
//
//	*((uint32_t *)CHAR_DISP_BASEADDR + 2) = regval;
//}
/**
 * Index can be {0, 1}
 */
void disp_char(int index, char val, uint32_t x, uint32_t y) {
	uint32_t regval = (y << 22) | ((x & 0b1111111111) << 12) | val;

	*((uint32_t *)CHAR_DISP_BASEADDR + 2 + index) = regval;
}
void set_bg_color(uint32_t color) {
	*((uint32_t *) CHAR_DISP_BASEADDR + 1) = color;
}
void set_char_color(uint32_t color) {
	*((uint32_t *) CHAR_DISP_BASEADDR) = color;
}

#endif
