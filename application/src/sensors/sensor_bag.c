#include "includes.h"

LOG_MODULE_REGISTER(sensor_bag);//CONFIG_ZSW_SENSORS_LOG_LEVEL


/**Define ZBus Channel Listner: Battery Value Calculations**/
static void on_zbus_bty_calc_val_data_callback(const struct zbus_channel *chan);
ZBUS_CHAN_DECLARE(battery_value_data_chan);
ZBUS_LISTENER_DEFINE(sensorbag_bty_value_comm_lis, on_zbus_bty_calc_val_data_callback);
/**Define ZBus Channel Listner: Environment Sensor BME688**/
static void on_zbus_env_sensor_data_callback(const struct zbus_channel *chan);
ZBUS_CHAN_DECLARE(env_sensor_data_chan);
ZBUS_LISTENER_DEFINE(sensorbag_env_sensor_comm_lis, on_zbus_env_sensor_data_callback);
/**Define ZBus Channel Listner: IMU Sensor BMI270**/
static void on_zbus_imu_sensor_data_callback(const struct zbus_channel *chan);
ZBUS_CHAN_DECLARE(imu_sensor_data_chan);
ZBUS_LISTENER_DEFINE(sensorbag_imu_sensor_comm_lis, on_zbus_imu_sensor_data_callback);

/**Define Mutex**/
K_MUTEX_DEFINE(sensor_bag_mutex);

static Sensor_Bag_s sensr_info;

void sensor_bag_init(void){
    memset(&sensr_info,0,sizeof(sensr_info));
}

void fetch_sensor_bag(Sensor_Bag_s *info){
    k_mutex_lock(&sensor_bag_mutex, K_FOREVER);
    *info = sensr_info;
    k_mutex_unlock(&sensor_bag_mutex);
}

static void on_zbus_bty_calc_val_data_callback(const struct zbus_channel *chan){
    const struct battery_value_event *bty = zbus_chan_const_msg(chan);
    k_mutex_lock(&sensor_bag_mutex, K_FOREVER);
    sensr_info.bty.mv = bty->mv;
    sensr_info.bty.percentage = bty->percentage;
    LOG_WRN("[SB] BTY: %d mv -> %u %%",sensr_info.bty.mv,sensr_info.bty.percentage);
    k_mutex_unlock(&sensor_bag_mutex);
}

static void on_zbus_env_sensor_data_callback(const struct zbus_channel *chan){
    const struct env_sensor_event *info = zbus_chan_const_msg(chan);
    k_mutex_lock(&sensor_bag_mutex, K_FOREVER);
    sensr_info.env_snsr.temperature = info->temperature;
    sensr_info.env_snsr.humidity = info->humidity;
    sensr_info.env_snsr.pressure = info->pressure;
    // LOG_WRN("[SB] temperature:%f,pressure:%f,humidity:%f",sensr_info.env_snsr.temperature,sensr_info.env_snsr.pressure,sensr_info.env_snsr.humidity);
    k_mutex_unlock(&sensor_bag_mutex);
}

static void on_zbus_imu_sensor_data_callback(const struct zbus_channel *chan){
    const struct imu_sensor_event_s *info = zbus_chan_const_msg(chan);
    k_mutex_lock(&sensor_bag_mutex, K_FOREVER);
    switch(info->type){
        case IMU_SNSR_EVT_TYPE_ACC_XYZ:
            sensr_info.imu_snsr.xyz = info->data.xyz; 
            break;
        case IMU_SNSR_EVT_TYPE_STEP:
            sensr_info.imu_snsr.step = info->data.step; 
            break;
        case IMU_SNSR_EVT_TYPE_STEP_ACTIVITY:
            sensr_info.imu_snsr.step_activity = info->data.step_activity; 
            break;
        case IMU_SNSR_EVT_TYPE_GESTURE:
            sensr_info.imu_snsr.gesture = info->data.gesture; 
            break;
        case IMU_SNSR_EVT_TYPE_SIGNIFICANT_MOTION:
            break;
        case IMU_SNSR_EVT_TYPE_WRIST_WAKEUP:
            break;
        case IMU_SNSR_EVT_TYPE_NO_MOTION:
            break;
        case IMU_SNSR_EVT_TYPE_ANY_MOTION:
            break;
        default:break;
    }
    k_mutex_unlock(&sensor_bag_mutex);
}