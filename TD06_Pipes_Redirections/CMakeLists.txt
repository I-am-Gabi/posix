cmake_minimum_required(VERSION 3.3)
project(TD06_Pipes_Redirections)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES tst_redirect.c)
add_executable(TD06_Pipes_Redirections ${SOURCE_FILES})