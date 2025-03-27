#ifndef IMU_SENSOR_EVENT
#define IMU_SENSOR_EVENT

#include "sensors/imu_sensor.h"

struct imu_sensor_event_s{
    imu_sensor_evt_type_t type;
    union {
        imu_sensor_data_xyz_t            xyz;
        imu_sensor_data_step_t           step;
        imu_sensor_data_step_activity_t  step_activity;
        imu_sensor_data_step_gesture_t   gesture;
    } data;
};


#endif /* IMU_SENSOR_EVENT */
