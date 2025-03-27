cmake_minimum_required(VERSION 3.20.0)

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})


message(STATUS "Current source directory: ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(shiars_band_app)
zephyr_include_directories(${ZEPHYR_BASE}/modules/lvgl)

add_subdirectory(drivers)


include_directories(src/)
# add_subdirectory(drivers)


# Include the source files from the main folder
FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

# Include source and header files from the subdirectories:
FILE(GLOB_RECURSE SOURCE_FILES 
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.c"
    )

# Add source to the app target
target_sources(app PRIVATE ${SOURCE_FILES})