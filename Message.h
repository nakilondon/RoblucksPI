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
    OPERATION = 6,
   // RECEIVED = 6,
    LOG = 7,
    ERROR = 8
};

enum Operation {
    SET_LOG_LEVEL = 1,
    TURN_DISTANCES_OFF = 2,
    TURN_DISTANCES_ON = 3
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


enum DistanceSensor {
    SENSOR_FRONT = 1,
    SENSOR_BACK = 2,
    SENSOR_LEFT = 3,
    SENSOR_RIGHT = 4,

};

#endif