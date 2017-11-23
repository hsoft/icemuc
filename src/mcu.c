#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "mcu.h"
#include "util.h"

// this is the number of usecs that elapses at each runloop pass. If our simulation does a pass
// faster than this, we wait a little bit before continuing.
#define MCU_TIME_RESOLUTION 50

/* Private */
static time_t timestamp() {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
}

static void mcu_timer_elapse(MCUTimer *timer, time_t usecs)
{
    timer->elapsed += usecs;
    if (timer->elapsed >= timer->every) {
        timer->elapsed -= timer->every;
        timer->func();
    }
}

static void mcu_pinchange(Pin *pin)
{
    int pinindex;
    MCU *mcu;

    pinindex = icemu_pinlist_find(&pin->chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (MCU *)pin->chip->logical_unit;
        if (mcu->interrupts[pinindex] != NULL) {
            mcu->interrupts[pinindex]();
        }
    }
}

static MCU* mcu_new(Chip *chip, const char **codes)
{
    MCU *mcu;
    uint8_t count;
    uint8_t i;

    count = icemu_util_chararray_count(codes);
    mcu = (MCU *)malloc(sizeof(MCU));
    mcu->epoch = timestamp();
    mcu->ticks = 0;
    memset(mcu->interrupts, 0, sizeof(InterruptFunc) * MAX_INTERRUPTS);
    memset(mcu->timers, 0, sizeof(MCUTimer) * MAX_TIMERS);
    icemu_chip_init(chip, (void *)mcu, mcu_pinchange, count);
    for (i = 0; i < count; i++) {
        icemu_chip_addpin(chip, codes[i], false, false);
    }
    return mcu;
}

/* Public */
void icemu_mcu_add_interrupt(Chip *chip, Pin *pin, InterruptFunc interrupt)
{
    int pinindex;
    MCU *mcu;

    pinindex = icemu_pinlist_find(&chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (MCU *)chip->logical_unit;
        mcu->interrupts[pinindex] = interrupt;
    }
}

void icemu_mcu_add_timer(Chip *chip, time_t every_usecs, TimerFunc timer_func)
{
    uint8_t i;
    MCU *mcu;

    mcu = (MCU *)chip->logical_unit;
    for (i = 0; i < MAX_TIMERS; i++) {
        if (mcu->timers[i].func == NULL) {
            mcu->timers[i].every = every_usecs;
            mcu->timers[i].func = timer_func;
            break;
        }
    }
}

void icemu_mcu_tick(Chip *chip)
{
    time_t clock_target, ts;
    uint8_t i;
    MCU *mcu;

    mcu = (MCU *)chip->logical_unit;
    mcu->ticks++;
    clock_target = mcu->epoch + (mcu->ticks * MCU_TIME_RESOLUTION);
    ts = timestamp();
    if (ts < clock_target) {
        usleep(clock_target - ts);
    }
    for (i = 0; i < MAX_TIMERS; i++) {
        if (mcu->timers[i].func == NULL) {
            break;
        }
        mcu_timer_elapse(&mcu->timers[i], MCU_TIME_RESOLUTION);
    }
}

void icemu_ATtiny_init(Chip *chip)
{
    const char * codes[] = {"PB0", "PB1", "PB2", "PB3", "PB4", "PB5", NULL};

    mcu_new(chip, codes);
}
