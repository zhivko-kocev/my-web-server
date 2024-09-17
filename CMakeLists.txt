cmake_minimum_required(VERSION 3.29)
project(my_web C)

set(CMAKE_C_STANDARD 17)

add_executable(my_web server.c
        server_utils.c
        string_utils.c
)

add_executable(test test.c)
