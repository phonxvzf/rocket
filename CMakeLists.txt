cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(rocket)

# Libraries
find_package(SDL2 REQUIRED)
file(GLOB SOURCES "src/*.cpp")

# Global configurations
set(BINARY rocket)

# Compilatlon
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -ggdb -pipe -Wall")

add_executable(${BINARY} ${SOURCES})

target_compile_definitions(${BINARY} PRIVATE SIM_SIZE=$ENV{SIM_SIZE})
target_include_directories(${BINARY} PRIVATE include)
target_link_libraries(${BINARY} SDL2)
