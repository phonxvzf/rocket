cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(rocket)

# Libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Global configurations
set(BINARY rocket)
set(SIM_SIZE $ENV{SIM_SIZE})
if (NOT SIM_SIZE)
  set(SIM_SIZE 200)
endif()

# Compilatlon
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -march=native -mtune=native -O3 -ffast-math -ggdb -pipe -Wall")

add_executable(${BINARY} ${SOURCES})

target_compile_definitions(${BINARY} PRIVATE SIM_SIZE=${SIM_SIZE})
target_include_directories(${BINARY} PRIVATE include)
target_link_libraries(${BINARY} SDL2 SDL2_image)
