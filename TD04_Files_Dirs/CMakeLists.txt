cmake_minimum_required(VERSION 3.3)
project(TD04_Files_Dirs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(TD04_Files_Dirs ${SOURCE_FILES})