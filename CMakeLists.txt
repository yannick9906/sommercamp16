cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME sommercamp16)
project(${PROJECT_NAME})
include_directories(/home/guest075/Arduino/libraries/Adafruit-GFX-Library-1.1.5/)
include_directories(/home/guest075/Arduino/libraries/Adafruit_LED_Backpack-1.1.3/)
include_directories(/home/guest075/Arduino/libraries/Adafruit_Sensor-master/)
include_directories(/home/guest075/Arduino/libraries/Adafruit_HMC5883_Unified-master/)
set(${CMAKE_PROJECT_NAME}_BOARD uno)
set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)

set(${CMAKE_PROJECT_NAME}_SKETCH sommercamp16.ino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})

