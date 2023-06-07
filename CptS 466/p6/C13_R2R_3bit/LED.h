#include "tm4c123gh6pm.h"

typedef enum color_t {
	DARK = 0,
	RED = 0x02,
	BLUE = 0x04,
	GREEN = 0x08,
	PINK = 0x06,
	YELLOW = (0x08 | 0x02)
} COLOR;

void SetLED(enum color_t color) {
	GPIO_PORTF_DATA_R = color;
}
