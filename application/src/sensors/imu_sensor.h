#ifndef IMU_SENSOR
#define IMU_SENSOR

#include <zephyr/drivers/sensor.h>

#include "../../drivers/sensor/bmi270/bosch_bmi270.h"

typedef enum {
    IMU_SNSR_FEATURE_ACCEL = BOSCH_BMI270_FEAT_ACCEL,
    IMU_SNSR_FEATURE_GYRO = BOSCH_BMI270_FEAT_GYRO,
    IMU_SNSR_FEATURE_STEP_COUNTER = BOSCH_BMI270_FEAT_STEP_COUNTER,
    IMU_SNSR_FEATURE_SIG_MOTION = BOSCH_BMI270_FEAT_SIG_MOTION,
    IMU_SNSR_FEATURE_ANY_MOTION = BOSCH_BMI270_FEAT_ANY_MOTION,
    IMU_SNSR_FEATURE_STEP_ACTIVITY = BOSCH_BMI270_FEAT_STEP_ACTIVITY,
    IMU_SNSR_FEATURE_WRIST_GESTURE = BOSCH_BMI270_FEAT_WRIST_GESTURE,
    IMU_SNSR_FEATURE_WRIST_WEAR_WAKE_UP = BOSCH_BMI270_FEAT_WEAR_WAKE_UP,
    IMU_SNSR_FEATURE_NO_MOTION = BOSCH_BMI270_FEAT_NO_MOTION,
} imu_sensor_feature_t;

typedef enum {
    IMU_SNSR_EVT_TYPE_ACC_XYZ,
    IMU_SNSR_EVT_TYPE_GYR_XYZ,
    IMU_SNSR_EVT_TYPE_DOOUBLE_TAP,
    IMU_SNSR_EVT_TYPE_STEP,
    IMU_SNSR_EVT_TYPE_STEP_ACTIVITY,
    IMU_SNSR_EVT_TYPE_TILT,
    IMU_SNSR_EVT_TYPE_GESTURE,
    IMU_SNSR_EVT_TYPE_SIGNIFICANT_MOTION,
    IMU_SNSR_EVT_TYPE_WRIST_WAKEUP,
    IMU_SNSR_EVT_TYPE_NO_MOTION,
    IMU_SNSR_EVT_TYPE_ANY_MOTION
} imu_sensor_evt_type_t;

typedef enum {
    IMU_SNSR_EVT_STEP_ACTIVITY_STILL,
    IMU_SNSR_EVT_STEP_ACTIVITY_WALK,
    IMU_SNSR_EVT_STEP_ACTIVITY_RUN,
    IMU_SNSR_EVT_STEP_ACTIVITY_UNKNOWN,
} imu_sensor_data_step_activity_t;

typedef enum {
    IMU_SNSR_EVT_GESTURE_UNKNOWN,
    IMU_SNSR_EVT_GESTURE_PUSH_ARM_DOWN,
    IMU_SNSR_EVT_GESTURE_PIVOT_UP,
    IMU_SNSR_EVT_GESTURE_WRIST_SHAKE,
    IMU_SNSR_EVT_GESTURE_FLICK_IN,
    IMU_SNSR_EVT_GESTURE_FLICK_OUT
} imu_sensor_data_step_gesture_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} imu_sensor_data_xyz_t;

typedef struct {
    uint32_t count;
} imu_sensor_data_step_t;

typedef struct {
    imu_sensor_evt_type_t type;
    union {
        imu_sensor_data_xyz_t            xyz;
        imu_sensor_data_step_t           step;
        imu_sensor_data_step_activity_t  step_activity;
        imu_sensor_data_step_gesture_t   gesture;
    } data;
} imu_sensor_evt_t;


int imu_sensor_init(void);

void imu_sensor_activate(bool status);

/**
 * @brief Fetch the values of 3-axis accelerometer either in float/int16_t datatype.
 * @param x can be either float or int16_t
 * @param y can be either float or int16_t
 * @param z can be either float or int16_t 
 * @param is_float true: param x,y,z are float & false: param x,y,z are int16_t
 * @return 0 if success else a negative value
*/
int imu_sensor_fetch_accel(void *x,void *y,void *z,bool is_float);

/**
 * @brief Fetch the values of 3-axis gyroscope either in float/int16_t datatype.
 * @param x can be either float or int16_t
 * @param y can be either float or int16_t
 * @param z can be either float or int16_t 
 * @param is_float true: param x,y,z are float & false: param x,y,z are int16_t
 * @return 0 if success else a negative value
*/
int imu_sensor_fetch_gyro(void *x,void *y,void *z,bool is_float);

int imu_sensor_fetch_temperature(float *temperature);

/**
 * @brief Fetch the number of step activity 0:"STILL", 1:"WALK", 2:"RUN", 3:"UNKNOWN"
 * @param activity variable to retrieve value
 * @param is_dev_chk_req must be 'false' if called from 'bmi270_trigger_handler() & must be true if called from elsewhere
 * @return 0 if success else a negative value
*/
int imu_sensor_fetch_step_activity(uint8_t *activity,bool is_dev_chk_req);

/**
 * @brief Fetch the number of wrist gesture 0:"unknown", 1:"push_arm_down", 2:"pivot_up", 3:"wrist_shake_jiggle", 4:"flick_in", 5"flick_out"
 * @param activity variable to retrieve value
 * @param is_dev_chk_req must be 'false' if called from 'bmi270_trigger_handler() & must be true if called from elsewhere
 * @return 0 if success else a negative value
*/
int imu_sensor_fetch_wrist_gesture(uint8_t *gesture,bool is_dev_chk_req);

/**
 * @brief Fetch the number of steps counted by the sensor
 * @param num_steps variable to retrieve value
 * @param is_dev_chk_req must be 'false' if called from 'bmi270_trigger_handler() & must be true if called from elsewhere
 * @return 0 if success else a negative value
*/
int imu_sensor_fetch_num_steps(uint32_t *num_steps,bool is_dev_chk_req);

/**
 * @brief To reset the value of the step counter inside the sensor
 * @todo might need to pass a value to make the threshold incase wrong counts are generated.
*/
int imu_sensor_reset_step_count(void);

/**
 * @brief Disables a feature
 * @param feature Required feature
*/
int imu_sensor_feature_disable(imu_sensor_feature_t feature);

/**
 * @brief Enables a feature with/without interrput callback
 * @param feature Required feature
 * @param int_en enable/disable interrupt
*/
int imu_sensor_feature_enable(imu_sensor_feature_t feature, bool int_en);

#endif /* IMU_SENSOR */
