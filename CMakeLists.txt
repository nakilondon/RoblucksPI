cmake_minimum_required(VERSION 3.13)
project(roblucksPI)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(roblucksPI main.cpp
        SerialIO/SerialIO.cpp SerialIO/SerialIO.h
        Joystick/Joystick.cpp Joystick/Joystick.h
        Joystick/RoblucksJoystick.cpp Joystick/RoblucksJoystick.h
        ManualControl/ManualControl.cpp ManualControl/ManualControl.cpp
        AutonomousControl/AutonomousControl.cpp AutonomousControl/AutonomousControl.h
        ControlServoMotor/ControlServoMotor.cpp ControlServoMotor/ControlServoMotor.h
        Timer.h Timer.cpp
        )

target_link_libraries(${PROJECT_NAME} Threads::Threads)