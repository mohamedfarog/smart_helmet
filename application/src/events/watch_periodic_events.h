#ifndef WATCH_PERIODIC_EVENTS
#define WATCH_PERIODIC_EVENTS

#include <zephyr/zbus/zbus.h>
#include "events/periodic_event_def.h"

/**
 * Add channel observators
*/
int watch_periodic_chan_add_obs(const struct zbus_channel *chan, const struct zbus_observer *obs,bool req_now);

/**
 * Remove channel observators
*/
int watch_periodic_chan_rm_obs(const struct zbus_channel *chan, const struct zbus_observer *obs);


#endif /* WATCH_PERIODIC_EVENTS */
