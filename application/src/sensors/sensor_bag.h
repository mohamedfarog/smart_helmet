#ifndef SENSOR_BAG
#define SENSOR_BAG

typedef struct{
    imu_sensor_data_xyz_t            xyz;
    imu_sensor_data_step_t           step;
    imu_sensor_data_step_activity_t  step_activity;
    imu_sensor_data_step_gesture_t   gesture;
}imu_info_s;

typedef struct{
    struct env_sensor_event env_snsr;
    imu_info_s imu_snsr;
    struct battery_value_event bty;
}Sensor_Bag_s;

void sensor_bag_init(void);

void fetch_sensor_bag(Sensor_Bag_s *info);

#endif /* SENSOR_BAG */
