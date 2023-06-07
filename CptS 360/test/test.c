
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ESP32_BUFFER_SIZE 32
#define ESP32_SCROLLBACK_SIZE 2

typedef struct esp32state {
	volatile char buffers[ESP32_SCROLLBACK_SIZE][ESP32_BUFFER_SIZE];
	volatile int char_index;
	volatile int buffer_index;
} ESP32;

void esp32_init(ESP32 *inst) {
	inst->char_index = 0;
	inst->buffer_index = 0;
}

void recv_char(ESP32 *inst, char c) {
	if (inst->char_index < ESP32_BUFFER_SIZE - 1) {
		char* dest = &inst->buffers[inst->buffer_index][inst->char_index++];
		switch(c) {
		default:
			//buffer[inst->char_index++] = c;
			*dest = c;
			break;
		case '\r':
			*dest = ' ';
			break;
		case '\n':
			*dest = '\0';
			break;
		}

		if (c == '\n') {
			inst->char_index = 0;
			inst->buffer_index++;
			if (inst->buffer_index >= ESP32_SCROLLBACK_SIZE) {
				inst->buffer_index = 0;
			}
		}
	} else {
		printf("overflow!\n\r");
	}
}

int main()
{
	ESP32 esp32;
	esp32_init(&esp32);
    char *str = "hello world!\r\nthis is a test!\r\nbeep boop\r\n";
	int length = strlen(str);
	for(int i = 0; i < length; i++) {
		recv_char(&esp32, str[i]);
	}
    return 0;
}

