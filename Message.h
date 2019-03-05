//
// Created by james on 16/02/19.
//
#ifndef ROBLUCKSPI_MESSAGE_H
#define ROBLUCKSPI_MESSAGE_H

enum Message {
    HELLO = 1,
    SERVO = 2,
    MOTOR = 3,
    ALREADY_CONNECTED = 4,
    DISTANCE = 5,
    ERROR = 6,
   // RECEIVED = 6,
    LOG = 7
};

//typedef enum Message Message;

enum MotorCmd {
    FORWARD = 1,
    REVERSE = 2,
    STOP = 3,
    ARM = 4
};

//typedef enum MotorCmd MotorCmd;

enum ServoCmd{
    CENTER = 1,
    LEFT = 2,
    RIGHT = 3
};

//typedef enum  ServoCmd ServoCmd;

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4
};

enum Direction {
    DIRECTION_FRONT = 1,
    DIRECTION_BACK = 2,
    DIRECTION_LEFT = 3,
    DIRECTION_RIGHT = 4,
};

#endif