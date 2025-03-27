#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include "events/periodic_event_def.h"

static struct k_work_delayable periodic_5min_work;
static struct k_work_delayable periodic_ultraslow_work;
static struct k_work_delayable periodic_slow_work;
static struct k_work_delayable periodic_mid_work;
static struct k_work_delayable periodic_fast_work;


ZBUS_CHAN_DEFINE(periodic_event_5min_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_5min_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );

ZBUS_CHAN_DEFINE(periodic_event_30s_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_ultraslow_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );

ZBUS_CHAN_DEFINE(periodic_event_10s_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_slow_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );

ZBUS_CHAN_DEFINE(periodic_event_1s_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_mid_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );        

ZBUS_CHAN_DEFINE(periodic_event_100ms_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_fast_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );
