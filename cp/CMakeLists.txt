cmake_minimum_required(VERSION 3.3)
project(CP_Cavalcante)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W")

link_directories(/usr/local/lib)

add_executable (
        cp_exe

        main_parser.c
        source/cp.c
)
target_link_libraries (
        cp_exe
)