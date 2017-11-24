#include <stdio.h>
#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static Chip mcu;
static Pin *pb1;

/* main.c decl */
void setup();
void loop();

/* layer impl */
void pinhigh()
{
    icemu_pin_set(pb1, true);
}

void pinlow()
{
    icemu_pin_set(pb1, false);
}

void pinoutputmode()
{
}

void _delay_ms(unsigned int ms)
{
    icemu_sim_delay(ms * 1000);
}

/* Simulation */

int main()
{
    icemu_ATtiny_init(&mcu);
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    setup();
    icemu_sim_init(50, loop);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_sim_run();
}
