# add_subdirectory(display)
# add_subdirectory(kscan)
add_subdirectory_ifdef(CONFIG_SENSOR sensor)