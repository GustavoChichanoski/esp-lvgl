# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if (EXISTS $ENV{IDF_PATH})
    set(IDF_PATH $ENV{IDF_PATH})
else()
    set(IDF_PATH C:/Espressif)
endif()

set(CMAKE_TOOLCHAIN_FILE "${IDF_PATH}/tools/cmake/toolchain-esp32.cmake" CACHE STRING "ESP32 toolchain file")


# set(CMAKE_C_COMPILER ${IDF_TOOLS_PATH}/tools/xtensa-esp-elf/esp-14.2.0_20241119/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc.exe)
include(${IDF_PATH}/tools/cmake/project.cmake)
project(esp32-lcd)
