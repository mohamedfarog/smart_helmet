#include "includes.h"

LOG_MODULE_REGISTER(stg,LOG_LEVEL_DBG);

// Mutex for protecting updates to App_Info_s
struct k_mutex app_info_mutex;

App_Info_s app_info;

static int stg_init_app_info(void){
    // Initialize mutex
    k_mutex_init(&app_info_mutex);
    memset(&app_info,0,sizeof(App_Info_s));
    LOG_INF("Storage Initialized");
    return 0;
}

void stg_update_battery_info(const struct battery_info *info){
    // Lock the mutex to ensure thread safety during update
    k_mutex_lock(&app_info_mutex, K_FOREVER);
    
    app_info.bty_info.mv = info->mv;
    app_info.bty_info.level = info->level;
    
    // Unlock the mutex after the update is complete
    k_mutex_unlock(&app_info_mutex);
}

void stg_update_bio_sensor_info(const struct bio_sensor_info *bio_info){
    // Lock the mutex to ensure thread safety during update
    k_mutex_lock(&app_info_mutex, K_FOREVER);
    
    app_info.bio_info.heartbeat = bio_info->heartbeat;
    app_info.bio_info.spo2 = bio_info->spo2;

    // Unlock the mutex after the update is complete
    k_mutex_unlock(&app_info_mutex);
}

void stg_fetch_all_info(App_Info_s *info){
    // Lock the mutex to ensure thread safety during retrieval
    k_mutex_lock(&app_info_mutex, K_FOREVER);

    memcpy(info, &app_info, sizeof(App_Info_s));

    // Unlock the mutex after the retrieval is complete
    k_mutex_unlock(&app_info_mutex);
}

SYS_INIT(stg_init_app_info,APPLICATION,CONFIG_APPLICATION_INIT_PRIORITY);