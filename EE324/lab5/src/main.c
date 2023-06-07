#include "char_disp.h"
#include "thing.h"

void wait() {
	for(int i = 0; i < 0xAFFFF; i++);
}

int main() {
	uint32_t bg = 0xCCAA00, fg = 0x0000FF;
	uint16_t x = 0, y = 0;
	set_bg_color(bg);
	set_char_color(fg);

	float i = 0;
	while(1) {
//		x = 100 + (int)(100.0 * lsin(i + 4.713)); /* add 3pi/2 to make it a cos */
//		y = 100 + (int)(100.0 * lsin(i));

		disp_char(0, 'H', (int)(50.0 * i),      100 + (int)(100.0 * lsin(i)));
		disp_char(1, 'I', (int)(50.0 * i) + 10, 100 + (int)(100.0 * lsin(i + 0.2)));

		i += 0.01;
		wait();
	}
	return 0;
}
