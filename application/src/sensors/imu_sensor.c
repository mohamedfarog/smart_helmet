
#include "sensor_defines.h"
#include "includes.h"

LOG_MODULE_REGISTER(imu_sensor,LOG_LEVEL_DBG);

const char *imu_activity_name[] = {"Still","Walk","Run","Unknown"};
const char *imu_arm_getsure_name[] = {"unknown","push_arm_down", "pivot_up", "wrist_shake_jiggle", "flick_in", "flick_out"};

static void zbus_periodic_ultra_slow_callback(const struct zbus_channel *chan);
static void zbus_periodic_medium_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(imu_sensor_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_30s_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);

ZBUS_LISTENER_DEFINE(imu_sensor_ev_lis, zbus_periodic_ultra_slow_callback);//For events
ZBUS_LISTENER_DEFINE(imu_sensor_periodic_lis, zbus_periodic_medium_callback);//For periodic data fetching

static const struct device *const bmi270 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmi270));

static struct sensor_trigger bmi270_trigger;
struct tm *clock_get_time(void);
#define MAX_PERIODIC_FTR_COUNT 5
static uint8_t periodic_features[] = {IMU_SNSR_FEATURE_ACCEL,IMU_SNSR_FEATURE_GYRO,
                                        IMU_SNSR_FEATURE_STEP_ACTIVITY,IMU_SNSR_FEATURE_STEP_COUNTER,
                                        IMU_SNSR_FEATURE_WRIST_GESTURE};
static uint32_t enabled_periodic_feature = 0;
static uint8_t next_per_feature_index = 0;


static void zbus_periodic_ultra_slow_callback(const struct zbus_channel *chan){
    struct tm *timeinfo = clock_get_time();
    if((timeinfo->tm_hour == 23) && (timeinfo->tm_min == 59) && (timeinfo->tm_sec>=30)){
        LOG_DBG("Resetting step counter");
        imu_sensor_reset_step_count();
        struct imu_sensor_event_s evt;
        memset(&evt,0,sizeof(evt));
        evt.type =  IMU_SNSR_EVT_TYPE_STEP;
        evt.data.step.count = 0;
        zbus_chan_pub(&imu_sensor_data_chan, &evt, K_MSEC(250));       
    }
}

static void zbus_periodic_medium_callback(const struct zbus_channel *chan){
    bool is_val_found = false;
    struct imu_sensor_event_s evt;
    switch(next_per_feature_index){
        case 0:
        if(enabled_periodic_feature&(0x01<<IMU_SNSR_FEATURE_ACCEL)){
            if(imu_sensor_fetch_accel(&evt.data.xyz.x,&evt.data.xyz.y,&evt.data.xyz.z,false) == 0){
                LOG_INF("[IMU] Acc:- x:%d y:%d z:%d",evt.data.xyz.x,evt.data.xyz.y,evt.data.xyz.z);
                evt.type = IMU_SNSR_EVT_TYPE_ACC_XYZ;
                is_val_found = true;
            }
            next_per_feature_index = 1;
            break;
        }
        case 1:
        if(enabled_periodic_feature&(0x01<<IMU_SNSR_FEATURE_GYRO)){
            if(imu_sensor_fetch_gyro(&evt.data.xyz.x,&evt.data.xyz.y,&evt.data.xyz.z,false) == 0){
                LOG_INF("[IMU] Gyr:- x:%d y:%d z:%d",evt.data.xyz.x,evt.data.xyz.y,evt.data.xyz.z);
                evt.type = IMU_SNSR_EVT_TYPE_GYR_XYZ;
                is_val_found = true;
            }
            next_per_feature_index = 2;
            break;
        }
        case 2:
        if(enabled_periodic_feature&(0x01<<IMU_SNSR_FEATURE_STEP_ACTIVITY)){
            
            if (imu_sensor_fetch_step_activity((uint8_t*)&evt.data.step_activity,true) == 0) {
                LOG_INF("[IMU] Step activity: %s", imu_activity_name[evt.data.step_activity]);
                evt.type = IMU_SNSR_EVT_TYPE_STEP_ACTIVITY;
                is_val_found = true;
            }
            next_per_feature_index = 3;
            break;
        }
        case 3:
        if(enabled_periodic_feature&(0x01<<IMU_SNSR_FEATURE_STEP_COUNTER)){
            if(imu_sensor_fetch_num_steps(&evt.data.step.count,true) == 0){
                evt.type = IMU_SNSR_EVT_TYPE_STEP;
                LOG_INF("[IMU] No of steps counted  = %u", evt.data.step.count);
                is_val_found = true;
            }
            next_per_feature_index = 4;
            break;
        }
        case 4:
        if(enabled_periodic_feature&(0x01<<IMU_SNSR_FEATURE_WRIST_GESTURE)){
            if (imu_sensor_fetch_wrist_gesture((uint8_t*)&evt.data.gesture,false) == 0){
                evt.type = IMU_SNSR_EVT_TYPE_GESTURE;
                LOG_INF("[IMU] Gesture detected: %s", imu_arm_getsure_name[evt.data.gesture]);
                is_val_found = true;
            }
        }
        next_per_feature_index = 0;
        break;
    }
    if(is_val_found)
        zbus_chan_pub(&imu_sensor_data_chan, &evt, K_MSEC(250));
}

static void bmi270_trigger_handler(const struct device *dev, const struct sensor_trigger *trig){
    bool is_valid = true;
    struct imu_sensor_event_s evt;
    LOG_WRN("BMI270 trigger handler. Type: %u", trig->type);
    switch ((int)trig->type) {
        case SENSOR_TRIG_SIG_MOTION:
            LOG_WRN("[IMU] Trigger: SENSOR_TRIG_SIG_MOTION"); 
            evt.type = IMU_SNSR_EVT_TYPE_SIGNIFICANT_MOTION;
            break;
        case SENSOR_TRIG_STEP: 
            LOG_WRN("[IMU] Trigger: SENSOR_TRIG_STEP");
            if(imu_sensor_fetch_num_steps(&evt.data.step.count,false) == 0){
                evt.type = IMU_SNSR_EVT_TYPE_STEP;
                LOG_WRN("No of steps counted  = %u", evt.data.step.count);
            }else{
                is_valid = false;
                LOG_ERR("[IMU] Error: Reading SENSOR_CHAN_STEPS");
            }
            break;
        case SENSOR_TRIG_ACTIVITY: 
            LOG_WRN("[IMU] Trigger: SENSOR_TRIG_ACTIVITY");
            if (imu_sensor_fetch_step_activity((uint8_t*)&evt.data.step_activity,false) == 0) {
                evt.type = IMU_SNSR_EVT_TYPE_STEP_ACTIVITY;
                LOG_WRN("Step activity: %s", imu_activity_name[evt.data.step_activity]);
            }else{
                is_valid = false;
                LOG_ERR("[IMU] Error: Reading SENSOR_CHAN_ACTIVITY");
            }
            break;
        case SENSOR_TRIG_WRIST_WAKE:
            LOG_WRN("[IMU] Trigger: SENSOR_TRIG_WRIST_WAKE");
            evt.type = IMU_SNSR_EVT_TYPE_WRIST_WAKEUP;
            break;
        case SENSOR_TRIG_WRIST_GESTURE: 
            LOG_WRN("[IMU] Trigger: SENSOR_TRIG_WRIST_GESTURE");
            if (imu_sensor_fetch_wrist_gesture((uint8_t*)&evt.data.gesture,false) == 0){
                evt.type = IMU_SNSR_EVT_TYPE_GESTURE;
                LOG_WRN("[IMU] Gesture detected: %s", imu_arm_getsure_name[evt.data.gesture]);
            }else{
                is_valid = false;
                LOG_ERR("[IMU] Error: Reading SENSOR_CHAN_GESTURE");
            }
            break;
        case SENSOR_TRIG_STATIONARY:
            LOG_WRN("[IMU] Trigger: SENSOR_TRIG_STATIONARY"); 
            evt.type = IMU_SNSR_EVT_TYPE_NO_MOTION;
            break;
        case SENSOR_TRIG_MOTION:
            LOG_WRN("[IMU] Trigger: SENSOR_TRIG_MOTION"); 
            evt.type = IMU_SNSR_EVT_TYPE_ANY_MOTION;
            break;
        default:
            LOG_WRN("[IMU] Trigger: UNKNOWN");
            is_valid = false;
            break;
    }
    if(is_valid)
        zbus_chan_pub(&imu_sensor_data_chan, &evt, K_MSEC(250));
}

int imu_sensor_init(void){
    if (!device_is_ready(bmi270)) {
        LOG_ERR("No IMU found!");
        return -ENODEV;
    }
    // The type doesn´t care when all channels were selected.
    // We use the global handler here, because we don´t want to
    // separate the callbacks for now.
    //bmi270_trigger.type = SENSOR_TRIG_MOTION;
    bmi270_trigger.chan = SENSOR_CHAN_ALL;

    sensor_trigger_set(bmi270, &bmi270_trigger, bmi270_trigger_handler);

    watch_periodic_chan_add_obs(&periodic_event_30s_chan, &imu_sensor_ev_lis,true);

    return 0;
}

/**
 * TODO: Add routines to sleep & wake the sensor based on the status
*/
void imu_sensor_activate(bool status){
    if(status){
        watch_periodic_chan_add_obs(&periodic_event_30s_chan, &imu_sensor_ev_lis,true);
    }else{
        watch_periodic_chan_rm_obs(&periodic_event_30s_chan, &imu_sensor_ev_lis);
    }
}

int imu_sensor_fetch_accel(void *x,void *y,void *z,bool is_float){
    struct sensor_value x_temp,y_temp,z_temp;
    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
        return -ENODATA;
    }

    if ((sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_X, &x_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_Y, &y_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_Z, &z_temp) != 0)) {
        return -ENODATA;
    }

    if(is_float){
        *(float*)x = sensor_value_to_float(&x_temp);
        *(float*)y = sensor_value_to_float(&y_temp);
        *(float*)z = sensor_value_to_float(&z_temp);
    }else{
        *(int16_t*)x = x_temp.val1;
        *(int16_t*)y = y_temp.val1;
        *(int16_t*)z = z_temp.val1;
    }

    return 0;
}

int imu_sensor_fetch_gyro(void *x,void *y,void *z,bool is_float){
    struct sensor_value x_temp,y_temp,z_temp;
    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
        return -ENODATA;
    }

    if ((sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_X, &x_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_Y, &y_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_Z, &z_temp) != 0)) {
        return -ENODATA;
    }

    if(is_float){
        *(float*)x = sensor_value_to_float(&x_temp);
        *(float*)y = sensor_value_to_float(&y_temp);
        *(float*)z = sensor_value_to_float(&z_temp);
    }else{
        *(int16_t*)x = x_temp.val1;
        *(int16_t*)y = y_temp.val1;
        *(int16_t*)z = z_temp.val1;
    }

    return 0;
}

int imu_sensor_fetch_temperature(float *temperature){
    struct sensor_value sensor_val;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_AMBIENT_TEMP) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bmi270, SENSOR_CHAN_AMBIENT_TEMP, &sensor_val) != 0) {
        return -ENODATA;
    }

    *temperature = sensor_value_to_float(&sensor_val);

    return 0;
}

int imu_sensor_fetch_step_activity(uint8_t *activity,bool is_dev_chk_req){
    struct sensor_value sensor_val;

    if(is_dev_chk_req){
        if (!device_is_ready(bmi270)) {
            return -ENODEV;
        }
        if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
           return -ENODATA;
        }
    }

    if (sensor_channel_get(bmi270, SENSOR_CHAN_ACTIVITY, &sensor_val) != 0) {
        return -ENODATA;
    }
    
    *activity = (uint8_t)sensor_val.val1;
    return 0;
}

int imu_sensor_fetch_wrist_gesture(uint8_t *gesture,bool is_dev_chk_req){
    struct sensor_value sensor_val;

    if(is_dev_chk_req){
        if (!device_is_ready(bmi270)) {
            return -ENODEV;
        }
        if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
           return -ENODATA;
        }
    }

    if (sensor_channel_get(bmi270, SENSOR_CHAN_GESTURE, &sensor_val) != 0) {
        return -ENODATA;
    }

    *gesture = (uint8_t)sensor_val.val1;

    return 0;
}

int imu_sensor_fetch_num_steps(uint32_t *num_steps,bool is_dev_chk_req){
    struct sensor_value sensor_val;

    if(is_dev_chk_req){
        if (!device_is_ready(bmi270)) {
            return -ENODEV;
        }
        if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
           return -ENODATA;
        }
    }

    if (sensor_channel_get(bmi270, SENSOR_CHAN_STEPS, &sensor_val) != 0) {
        return -ENODATA;
    }

    *num_steps = sensor_val.val1;

    return 0;
}

int imu_sensor_reset_step_count(void){
    struct sensor_value value;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    value.val1 = 0;
    value.val2 = 0;

    if (sensor_attr_set(bmi270, SENSOR_CHAN_STEPS, SENSOR_ATTR_OFFSET, &value) != 0) {
        return -EFAULT;
    }

    return 0;
}

static bool is_the_feature_periodic(uint8_t feature){
    for(uint8_t i=0;i<MAX_PERIODIC_FTR_COUNT;i++){
        if(feature == periodic_features[i])
            return true;
    }
    return false;
}


int imu_sensor_feature_disable(imu_sensor_feature_t feature){
    struct sensor_value value;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    value.val1 = feature;
    value.val2 = BOSCH_BMI270_FEAT_DISABLE;

    if (sensor_attr_set(bmi270, SENSOR_CHAN_FEATURE, SENSOR_ATTR_CONFIGURATION, &value) != 0) {
        return -EFAULT;
    }
    if((is_the_feature_periodic((uint8_t)feature))&&(enabled_periodic_feature>0)){
        enabled_periodic_feature &= ~(0x01<<feature);
        if(enabled_periodic_feature==0){//Remove the channel observator
            watch_periodic_chan_rm_obs(&periodic_event_1s_chan, &imu_sensor_periodic_lis);
            // LOG_WRN("[IMU] Removed Observator: periodic_event_1s_chan");
        }
        // LOG_WRN("[IMU] Removed Periodic Features:%#x",enabled_periodic_feature);
    }
    return 0;
}
struct tm *clock_get_time(void) {
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv,NULL);
    tm = localtime(&tv.tv_sec);
    return tm;
}

int imu_sensor_feature_enable(imu_sensor_feature_t feature, bool int_en){
    int ret_val = 0;
    struct sensor_value value;
    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }
    value.val1 = feature;
    value.val2 = (int_en << 1) | BOSCH_BMI270_FEAT_ENABLE;
    if ((ret_val = sensor_attr_set(bmi270, SENSOR_CHAN_FEATURE, SENSOR_ATTR_CONFIGURATION, &value)) != 0) {
        LOG_WRN("[IMU] Error:%d sensor_attr_set",ret_val);
        return -EFAULT;
    }
    if(!int_en){
        if((is_the_feature_periodic((uint8_t)feature))){
            if(enabled_periodic_feature==0){//Add channel observator
                watch_periodic_chan_add_obs(&periodic_event_1s_chan, &imu_sensor_periodic_lis,false);
                // LOG_WRN("[IMU] Added Observator: periodic_event_1s_chan");
            }
            enabled_periodic_feature |= (0x01<<feature);
            // LOG_WRN("[IMU] Added Periodic Features:%#x",enabled_periodic_feature);   
        }
    }
    return 0;
}
