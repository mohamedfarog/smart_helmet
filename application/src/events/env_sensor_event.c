#include <zephyr/zbus/zbus.h>
#include "env_sensor_event.h"

ZBUS_CHAN_DEFINE(env_sensor_data_chan,
                 struct env_sensor_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(sensorbag_env_sensor_comm_lis),
                 ZBUS_MSG_INIT()
                );