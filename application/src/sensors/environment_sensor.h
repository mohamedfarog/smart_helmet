#ifndef ENVIRONMENT_SENSOR
#define ENVIRONMENT_SENSOR

int environment_sensor_init(void);

int environment_sensor_get(float *temperature, float *humidity, float *pressure);

void environment_sensor_activate(bool status);

#endif /* ENVIRONMENT_SENSOR */
