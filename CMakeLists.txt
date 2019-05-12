cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(rocket)

# Libraries
find_package(SDL2 REQUIRED)
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Global configurations
set(BINARY rocket)
set(SIM_SIZE $ENV{SIM_SIZE})
if (NOT SIM_SIZE)
  set(SIM_SIZE 200)
endif()

# Compilatlon
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -march=native -O2 -ffast-math -ggdb -pipe -Wall")

add_executable(${BINARY} ${SOURCES})

target_compile_definitions(${BINARY} PRIVATE SIM_SIZE=${SIM_SIZE})
target_include_directories(${BINARY} PRIVATE include)
target_link_libraries(${BINARY} SDL2)
