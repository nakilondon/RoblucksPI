cmake_minimum_required(VERSION 3.13)
project(roblucksPI)

set(CMAKE_CXX_STANDARD 14)

add_executable(roblucksPI main.cpp SerialIO/SerialIO.cpp SerialIO/SerialIO.h Joystick/Joystick.cpp Joystick/Joystick.h)