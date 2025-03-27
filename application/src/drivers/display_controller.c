// #include "includes.h"
// #include <zephyr/drivers/pwm.h>
// #include <zephyr/drivers/gpio.h>
// #include <zephyr/drivers/regulator.h>
// #include <zephyr/drivers/display.h>
// // #include <zephyr/drivers/counter.h>
// #include "lvgl.h"
// LOG_MODULE_REGISTER(display_control, LOG_LEVEL_WRN);


// static const struct pwm_dt_spec display_blk = PWM_DT_SPEC_GET_OR(DT_ALIAS(display_blk), {});
// #if CONFIG_USED_BOARD_ID == 0x01
//     static const struct device *const reg_dev = DEVICE_DT_GET_OR_NULL(DT_PATH(regulator_3v3_ctrl));
// #endif
// static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));


// static struct k_work_sync canel_work_sync;
// static bool is_on;
// static uint8_t last_brightness = 50;



// void display_control_init(void)
// {
//     if (!device_is_ready(display_dev)) {
//         LOG_ERR("Device display not ready.");
//     }
//     if (!device_is_ready(display_blk.dev)) {
//         LOG_WRN("Display brightness control not supported");
//     }
// #if CONFIG_USED_BOARD_ID == 0x01
//     if (!device_is_ready(reg_dev)) {
//         LOG_WRN("Display regulator control not supported");
//     }
// #endif
 
// }

// void display_control_power_on(bool on)
// {
//     if (on == is_on) {
//         return;
//     }
//     is_on = on;
//     if (on) {
//             // Turn on 3V3 regulator that powers display related stuff.
// #ifndef CONFIG_DISPLAY_FAST_WAKEUP
//     #if CONFIG_USED_BOARD_ID == 0x01
//         if (device_is_ready(reg_dev)) {
//             regulator_enable(reg_dev);
//             pm_device_action_run(display_dev, PM_DEVICE_ACTION_TURN_ON);
//         }
//     #endif
// #endif    
//         // Resume the display and touch chip
//         pm_device_action_run(display_dev, PM_DEVICE_ACTION_RESUME);
//         // Turn backlight on.
//         display_control_set_brightness(last_brightness);
//     } else {
//         // Suspend the display and touch chip
//         pm_device_action_run(display_dev, PM_DEVICE_ACTION_SUSPEND);
// #ifndef CONFIG_DISPLAY_FAST_WAKEUP
//     #if CONFIG_USED_BOARD_ID == 0x01
//         if (device_is_ready(reg_dev)) {
//             regulator_disable(reg_dev);
//             pm_device_action_run(display_dev, PM_DEVICE_ACTION_TURN_OFF);
//         }
//     #endif
// #endif    

//         // Turn off PWM peripheral as it consumes like 200-250uA
//         display_control_set_brightness(0);
//         // Cancel pending call to lv_task_handler
//         // Don't waste resosuces to rendering when display is off anyway.
//         // Prepare for next call to lv_task_handler when screen is enabled again,
//         // Since the display will have been powered off, we need to tell LVGL
//         // to rerender the complete display.
//         lv_obj_invalidate(lv_scr_act());
//     }
// }

// uint8_t display_control_get_brightness(void)
// {
//     return last_brightness;
// }

// void display_control_set_brightness(uint8_t percent)
// {
//     if (!device_is_ready(display_blk.dev)) {
//         return;
//     }
//     __ASSERT(percent >= 0 && percent <= 100, "Invalid range for brightness, valid range 0-100, was %d", percent);
//     int ret;
//     uint32_t step = display_blk.period / 100;
//     uint32_t pulse_width = step * (100 - percent);

//     if (percent != 0) {
//         last_brightness = percent;
//     }
//     ret = pwm_set_pulse_dt(&display_blk, pulse_width);
//     __ASSERT(ret == 0, "pwm error: %d for pulse: %d", ret, pulse_width);
// }

