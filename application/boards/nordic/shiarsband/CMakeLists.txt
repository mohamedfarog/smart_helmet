if (CONFIG_BOARD_SHIARSBAND_NRF5340_CPUAPP OR CONFIG_BOARD_SHIARSBAND_NRF5340_CPUAPP_NS)
	zephyr_library()
	zephyr_library_sources(board.c)
endif()
