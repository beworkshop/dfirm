cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_SDK_FETCH_FROM_GIT on)
set(PICO_SDK_FETCH_FROM_GIT_TAG 2.1.1)
set(PICO_BOARD pico2 CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(dfirm C CXX ASM)

pico_sdk_init()


add_library(${PROJECT_NAME}_log_lib
    src/log.c
)
add_library(${PROJECT_NAME}_core_lib
    src/core.c
    src/led.c
    src/boot.c
)
add_library(${PROJECT_NAME}_protocols_lib
    src/protocols/uart.c
    src/protocols/iic.c
)
add_library(${PROJECT_NAME}_modules_lib
    src/modules/gps_check.c
    src/modules/gps_com.c
    src/modules/gps_parser.c
    src/modules/accelerometer.c
)

target_include_directories(${PROJECT_NAME}_log_lib PUBLIC include)
target_include_directories(${PROJECT_NAME}_core_lib PUBLIC include)
target_include_directories(${PROJECT_NAME}_protocols_lib PUBLIC include)
target_include_directories(${PROJECT_NAME}_modules_lib PUBLIC include)

target_link_libraries(${PROJECT_NAME}_log_lib
    pico_time
)

target_link_libraries(${PROJECT_NAME}_core_lib 
    pico_stdlib
    pico_multicore
    hardware_gpio
    pico_time
)

target_link_libraries(${PROJECT_NAME}_protocols_lib 
    pico_stdlib
    hardware_uart
    hardware_i2c
    pico_time
)

target_link_libraries(${PROJECT_NAME}_modules_lib 
    pico_stdlib
    ${PROJECT_NAME}_protocols_lib 
    ${PROJECT_NAME}_log_lib
)

add_executable(${PROJECT_NAME} src/main.c)


target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    ${PROJECT_NAME}_core_lib
    ${PROJECT_NAME}_protocols_lib
    ${PROJECT_NAME}_modules_lib
)

pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "0.1")

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})