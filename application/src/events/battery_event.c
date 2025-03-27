#include <zephyr/zbus/zbus.h>
#include "battery_event.h"

ZBUS_CHAN_DEFINE(battery_value_data_chan,
                 struct battery_value_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(sensorbag_bty_value_comm_lis),
                 ZBUS_MSG_INIT()
                );