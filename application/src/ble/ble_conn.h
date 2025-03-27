/*
 * Copyright (c) 2018-2019 Peter Bigot Consulting, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APPLICATION_BLE_H_
#define APPLICATION_BLE_H_

static void connected(struct bt_conn *conn, uint8_t err);

static void disconnected(struct bt_conn *conn, uint8_t reason);


static void security_changed(struct bt_conn *conn, bt_security_t level,
			     enum bt_security_err err);

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey);

static void auth_cancel(struct bt_conn *conn);


static void pairing_complete(struct bt_conn *conn, bool bonded);

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason);

static void app_led_cb(bool led_state);

static bool app_button_cb(void);

static void button_changed(uint32_t button_state, uint32_t has_changed);

static int init_button(void);

void init_ble();
void aes_encrypt(void);


#endif /* APPLICATION_BATTERY_H_ */