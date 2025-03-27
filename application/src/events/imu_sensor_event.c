#include <zephyr/zbus/zbus.h>
#include "includes.h"
//ZBUS_OBSERVERS(imu_sensor_val_comm_lis)
ZBUS_CHAN_DEFINE(imu_sensor_data_chan,
                 struct imu_sensor_event_s,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(sensorbag_imu_sensor_comm_lis),
                 ZBUS_MSG_INIT()
                );