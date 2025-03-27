#ifndef TEMP_SENSOR
#define TEMP_SENSOR

/*
 * Copyright (c) 2018 Analog Devices Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
#include <zephyr/sys/__assert.h>

#define DELAY_WITH_TRIGGER K_SECONDS(5)
#define DELAY_WITHOUT_TRIGGER K_SECONDS(1)

#define UCEL_PER_CEL 1000000
#define UCEL_PER_MCEL 1000
#define TEMP_INITIAL_CEL 21
#define TEMP_WINDOW_HALF_UCEL 500000


static int low_ucel;
static int high_ucel;


void sensor_init();

static const char *now_str(void);

static void trigger_handler(const struct device *dev,
			    const struct sensor_trigger *trigger);


static int sensor_set_attribute(const struct device *dev,
				enum sensor_channel chan,
				enum sensor_attribute attr, int value);


static bool temp_in_window(const struct sensor_value *val);

static int sensor_set_window(const struct device *dev,
			     const struct sensor_value *val);


static void process(const struct device *dev);



#endif /* IMU_SENSOR */