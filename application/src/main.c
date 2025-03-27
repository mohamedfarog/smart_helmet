#include "includes.h"
#include "sensors/sensor_defines.h"


LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);
// static lv_obj_t *label_hello;

typedef enum ui_state {
    UI_STATE_INIT,          // Initialized State
    UI_STATE_DISP_OFF,      // Display off state
    UI_STATE_WATCHFACE,     // Watchface or home page
    UI_STATE_APP_MNGR,      // Application Manager
    UI_STATE_WF_SELCTR,     // Watchface selector
} ui_state_t;


uint64_t last_user_activity_dtctd_time = 0;

static void fetch_and_log_sensor_data(void) {
    Sensor_Bag_s sensor_info;
    fetch_sensor_bag(&sensor_info);  // Fetch sensor data

    LOG_DBG("Accel: x=%d, y=%d, z=%d", sensor_info.imu_snsr.xyz.x, sensor_info.imu_snsr.xyz.y, sensor_info.imu_snsr.xyz.z);
    LOG_DBG("Activity: %s", imu_activity_name[sensor_info.imu_snsr.step_activity]);
    LOG_DBG("Steps: %u", sensor_info.imu_snsr.step.count);

    // LOG_DBG("Battery: %u%% (%d mV)", sensor_info.bty.percentage, sensor_info.bty.mv);
}

/* Initialization work */
static void run_init_work(struct k_work *item) {
   ARG_UNUSED(item); // Prevents generation of warning

}

K_WORK_DEFINE(init_work, run_init_work);

/* Watch App Management */
K_THREAD_STACK_DEFINE(t_app_mngmnt_stack_area, STACKSIZE);
static struct k_thread t_app_mngmnt_task;


int main(void) {
    k_thread_create(&t_app_mngmnt_task, t_app_mngmnt_stack_area, K_THREAD_STACK_SIZEOF(t_app_mngmnt_stack_area),
                    t_watch_app_mngmnt_monitor, NULL, NULL, NULL, PRIORITY, 0, K_FOREVER);

    k_thread_name_set(&t_app_mngmnt_task, "t_watch_app_mngmnt_monitor");
    k_thread_start(&t_app_mngmnt_task);
    k_work_submit(&init_work);
    sensor_init();
    init_ble();
    return 0;
}

void t_watch_app_mngmnt_monitor(void *arg1, void *arg2, void *arg3) {
    ARG_UNUSED(arg1); // Prevents generation of warning
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);


    #if CONFIG_USED_BOARD_ID == BOARD_ID_AWAN_WATCH
    environment_sensor_init();
    imu_sensor_init();
    imu_sensor_feature_enable(IMU_SNSR_FEATURE_ACCEL,false);
    imu_sensor_feature_enable(IMU_SNSR_FEATURE_STEP_ACTIVITY,false);
    imu_sensor_feature_enable(IMU_SNSR_FEATURE_STEP_COUNTER,false);
    #endif

    while (true) {
        LOG_DBG("RUN FOR YOUR LIFE");
        fetch_and_log_sensor_data();
        battery_int();
        k_msleep(5000); // Sleep for 5 seconds before fetching the data again
    }
}
