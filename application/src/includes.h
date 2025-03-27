#ifndef INCLUDES_H_
#define INCLUDES_H_

/*Standard Headers*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
// #include <mbedtls/aes.h>
/*ZEPHYR Headers*/
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/types.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/reboot.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/data/json.h>
#include <zephyr/storage/flash_map.h>

#include <zephyr/zbus/zbus.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/sensor.h>

#include <zephyr/settings/settings.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/fatal.h>
#include <zephyr/retention/bootmode.h>
/*USER Headers*/
#include "defines.h"

#include "defines.h"
#include "storage/storage.h"
#include "battery/battery.h"
#include "json_struct_defines.h"


#include "events/watch_periodic_events.h"
#include "events/env_sensor_event.h"
#include "events/imu_sensor_event.h"
#include "events/battery_event.h"

// #include "drivers/display_controller.h"
#include "events/periodic_event_def.h"
#include "battery/battery.h"
#include "ble/ble_conn.h"


#include "sensors/environment_sensor.h"
#include "sensors/imu_sensor.h"


#include "sensors/sensor_bag.h"
#include "sensors/temp_sensor.h"


#include "main.h"
#endif /* !INCLUDES_H_ */
