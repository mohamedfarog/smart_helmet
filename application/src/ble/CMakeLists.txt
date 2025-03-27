FILE(GLOB ble_conn_sources *.c)
target_sources(app PRIVATE ${ble_conn_sources})