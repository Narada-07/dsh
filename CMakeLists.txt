cmake_minimum_required(VERSION 3.13)

project(dsh C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES
    src/*.c
)

add_executable(dsh ${SOURCE_FILES})

target_include_directories(dsh PRIVATE include)