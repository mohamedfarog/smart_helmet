#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/sensor.h>
#include "sensors/environment_sensor.h"
#include "events/watch_periodic_events.h"
#include "events/env_sensor_event.h"

LOG_MODULE_REGISTER(environment_sensor,LOG_LEVEL_DBG);

static void zbus_periodic_slow_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(env_sensor_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);

ZBUS_LISTENER_DEFINE(env_sensor_ev_lis, zbus_periodic_slow_callback);

static const struct device *const bme688 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bme688));

static void zbus_periodic_slow_callback(const struct zbus_channel *chan){
    float temperature,pressure,humidity;
    if (environment_sensor_get(&temperature, &humidity, &pressure)) {
        return;
    }
    struct env_sensor_event evt = {
        .temperature = temperature,
        .humidity = humidity,
        .pressure = pressure,
    };
    // LOG_WRN("temperature:%f,pressure:%f,humidity:%f",temperature,pressure,humidity);
    zbus_chan_pub(&env_sensor_data_chan, &evt, K_MSEC(250));
}

int environment_sensor_init(void){
    if (!device_is_ready(bme688)) {
        LOG_ERR("No environment sensor found!");
        return -ENODEV;
    }
    watch_periodic_chan_add_obs(&periodic_event_10s_chan, &env_sensor_ev_lis,false);
    return 0;
}

/**
 * TODO: Add routines to sleep & wake the sensor based on the status
*/
void environment_sensor_activate(bool status){
    if(status){
        watch_periodic_chan_add_obs(&periodic_event_10s_chan, &env_sensor_ev_lis,false);
    }else{
        watch_periodic_chan_rm_obs(&periodic_event_10s_chan, &env_sensor_ev_lis);
    }
}

int environment_sensor_get(float *temperature, float *humidity, float *pressure){
    struct sensor_value sensor_val;

    if (!device_is_ready(bme688)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bme688) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bme688, SENSOR_CHAN_AMBIENT_TEMP, &sensor_val) != 0) {
        return -ENODATA;
    }
    *temperature = sensor_value_to_float(&sensor_val);

    if (sensor_channel_get(bme688, SENSOR_CHAN_HUMIDITY, &sensor_val) != 0) {
        return -ENODATA;
    }
    *humidity = sensor_value_to_float(&sensor_val);

    if (sensor_channel_get(bme688, SENSOR_CHAN_PRESS, &sensor_val) != 0) {
        return -ENODATA;
    }
    *pressure = sensor_value_to_float(&sensor_val);

    return 0;
}

