#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdint.h>

struct battery_info{
    uint16_t mv;//Battery voltage in mV
    uint16_t level;//Battery Level in 0--100 %
};

struct bio_sensor_info{
    uint8_t heartbeat;
    uint8_t spo2;
};

typedef struct{
    // TODO
    // imu_sensor_data_step_t           step;
    // imu_sensor_data_step_activity_t  step_activity;
    // imu_sensor_data_step_gesture_t   gesture; 
    struct battery_info bty_info;
    struct bio_sensor_info bio_info;
}App_Info_s;

/**
 * @brief Update the battery information safely.
 * @param info 'mv' Battery voltage in millivolts and 'level' Battery level in 0--100%.
 * @return None
 */
void stg_update_battery_info(const struct battery_info *info);

/**
 * @brief Update the bio sensor information safely.
 * @param bio_info 'heartbeat' Heart rate in beats per minute and 'spo2' SpO2 level in percentage.
 * @return None
 */
void stg_update_bio_sensor_info(const struct bio_sensor_info *bio_info);

/**
 * @brief Fetch all the stored information safely.
 * @param info contains the members of App_Info_s
 * @return None
 */
void stg_fetch_all_info(App_Info_s *info);


#endif /* !STORAGE_H_ */

