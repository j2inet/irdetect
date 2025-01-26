cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

add_executable(irdetect )

pico_generate_pio_header(irdetect ${CMAKE_CURRENT_LIST_DIR}/pio/squarewave.pio)
target_sources(irdetect PRIVATE main.cpp)
target_link_libraries(irdetect pico_stdlib pico_cyw43_arch_none hardware_pio)


pico_enable_stdio_usb(irdetect 1)
pico_enable_stdio_uart(irdetect 1)
pico_add_extra_outputs(irdetect)


# target_compile_definitions(irdetect PRIVATE
# 	CYW43_PIO_CLOCK_DIV_DYNAMIC=1
# )
