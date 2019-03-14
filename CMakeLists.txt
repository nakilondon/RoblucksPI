cmake_minimum_required(VERSION 3.13)
project(roblucksPI)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp
        SerialIO/SerialIO.cpp SerialIO/SerialIO.h
        Joystick/Joystick.cpp Joystick/Joystick.h
        Joystick/RoblucksJoystick.cpp Joystick/RoblucksJoystick.h
        Controls/ManualControl.cpp Controls/ManualControl.cpp
        Controls/AutonomousControl.h Controls/AutonomousControl.cpp
        Controls/ControlServo.cpp Controls/ControlServo.h
        Controls/ControlMotor.h Controls/ControlMotor.cpp
        Utils/Log.h Utils/Log.cpp
        Utils/Timer.h Utils/Timer.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(roblucksPI ${SOURCE_FILES} )

target_link_libraries(${PROJECT_NAME} Threads::Threads)