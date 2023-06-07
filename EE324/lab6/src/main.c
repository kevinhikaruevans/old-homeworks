#include "xparameters.h"
#include <stdlib.h>
#include <xtime_l.h>

#define RTM_CTRL  *((uint32_t *)XPAR_RTM_0_S00_AXI_BASEADDR)
#define RTM_VALUE *((uint32_t *)XPAR_RTM_0_S00_AXI_BASEADDR + 1)

// globals to store the reaction times:
int round = 0;
int rounds[3];

enum rtm_state {
	RTM_IDLE = 0,
	RTM_START = 0b0001,
	RTM_STOP = 0b0010,
	RTM_RESET = 0b0100,
	RTM_DISP = 0b1000
};

/**
 * Waits until a button is pressed, then returns the button number.
 */
int wait_for_button() {
	int r = 0;

	while(r == 0) {
		r = (RTM_VALUE >> 16) & 0b1111;
	}
	while(((RTM_VALUE >> 16) & 0b1111) == r) {
		/* do nothing */
	}
	return r;
}

/**
 * Just gets a time to give srand()
 */
int time() {
	XTime t;
	XTime_GetTime(&t);
	return (int)t;
}

/**
 * Waits a random time between 1 and 10 seconds
 */
void wait_random() {
	int wait_us = 1e6 + (rand() % (int)10e6);
	usleep(wait_us);
}

/**
 * Records the current time in the `rounds` array.
 */
void record_time() {
	rounds[round++] = RTM_VALUE;

	if (round == 3) {
		round = 0;
	}
}

/**
 * Calculates the average reaction time.
 */
int get_avg() {
	int total = 0;
	for(int i = 0; i < 3; i++)
		total += rounds[i];
	return total / 3;
}

int main() {
	int btn;
	srand(time());

	while(1) {
		btn = wait_for_button();

		// button 2, the "show average" button
		if (btn == 0b0010) {
			switch(RTM_CTRL & 0xF) {
			case RTM_RESET:
			case RTM_IDLE:
				RTM_CTRL = (get_avg() << 16) | (RTM_DISP);
				break;

			case RTM_DISP:
				RTM_CTRL = RTM_RESET;
				break;
			}
		}

		// button 1, the start/stop button
		if (btn == 0b0001) {
			switch(RTM_CTRL & 0xF) {
			case RTM_RESET:
			case RTM_IDLE:
				wait_random();
				RTM_CTRL = RTM_START;
				break;

			case RTM_START:
				RTM_CTRL = RTM_STOP;
				record_time();
				break;

			case RTM_STOP:
				RTM_CTRL = RTM_RESET;
				break;
			}
		}
	}

	return 0;
}
