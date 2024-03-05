cmake_minimum_required(VERSION 3.0)
project(rs422Test C)

set(CMAKE_C_STANDARD 99)

add_executable(rs422Test main.c mainTest.c SC513_can.h log.c log.h map.h map.c)
