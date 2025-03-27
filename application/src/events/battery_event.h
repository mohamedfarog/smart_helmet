#ifndef BATTERY_EVENT
#define BATTERY_EVENT

#include <stdint.h>

struct battery_value_event {
    int mv;
    uint8_t percentage;
};

#endif /* BATTERY_EVENT */
