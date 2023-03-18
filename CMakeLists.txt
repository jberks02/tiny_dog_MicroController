cmake_minimum_required(VERSION 3.25)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

include(pico_sdk_import.cmake)

# target_include_directories(json PRIVATE $ENV{C_LIBRARIESROOT}/picojson)

# set(SOURCES $ENV{C_LIBRARIESROOT}/picojson/picojson.h)
# include($ENV{C_LIBRARIESROOT}/picojson/picojson.h)

# set(JSON_Install OFF CACHE INTERNAL "")

# add_subdirectory(nlohmann_json)

# target_link_libraries(example PRIVATE nlohmann_json::nlohmann_json)

project(TinyDogMicroController VERSION 1.0.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(PICO_ENABLE_CXX_EXCEPTIONS 1)

add_executable(${PROJECT_NAME}
               main.cpp)

# set (CMAKE_CXX_FLAGS -fexceptions)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
                      pico_stdlib
                      hardware_gpio
                      hardware_i2c
                      hardware_spi
                      hardware_adc
                      hardware_uart
                      hardware_pwm
                      pico_multicore
                      )
