cmake_minimum_required(VERSION 3.25)
project(AUX C)

set(CMAKE_C_STANDARD 90)

include_directories(shell)

add_executable(AUX
        shell/auxSH.c
        shell/auxSH.h)
