#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "drivers/led/led.h"
#include "drivers/btnsw/btnsw.h"

int main()
{
    init_platform();

    while (1) {
        set_leds(get_buttons());
        set_rgbleds(get_switches());
    }

    cleanup_platform();
    return 0;
}
