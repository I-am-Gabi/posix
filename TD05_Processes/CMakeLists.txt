cmake_minimum_required(VERSION 3.3)
project(TD05_Processes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES exec_prop.c)
add_executable(TD05_Processes ${SOURCE_FILES})