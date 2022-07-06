cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(picoarcadestick C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(picoarcadestick src/main.c src/usb_descriptors.c)

target_include_directories(picoarcadestick PUBLIC include/)

target_link_libraries(picoarcadestick PUBLIC pico_stdlib tinyusb_device tinyusb_board)

pico_add_extra_outputs(picoarcadestick)

pico_enable_stdio_usb(picoarcadestick 0)
pico_enable_stdio_uart(picoarcadestick 1)