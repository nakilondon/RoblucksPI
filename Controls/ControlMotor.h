//
// Created by james on 06/03/19.
//

#ifndef ROBLUCKSPI_CONTROLMOTOR_H
#define ROBLUCKSPI_CONTROLMOTOR_H

#include "../Message.h"
#include "../parameters.h"
#include "../SerialIO/SerialIO.h"
#include <unistd.h>
#include <cstdint>
#include "../Utils/Timer.h"
#include "../Utils/Log.h"
#include <chrono>
#include <cstdint>
#include <inttypes.h>
#include "../Utils/Timer.h"


enum MotorCntl {
    MOTOR_NONE,
    MOTOR_MOVE_FORWARD,
    MOTOR_MOVE_BACK,
    MOTOR_STOP,
    MOTOR_START,
    MOTOR_INCREASE_SPEED,
    MOTOR_DECREASE_SPEED,
    MOTOR_BACKUP
};

enum Direction{
    D_FORWARD,
    D_BACK,
    D_STOP
};

class ControlMotor {

private:
    static uint8_t  _currentSpeed;
    static Direction _currentDirection;
    static SerialIO _arduinoIO;
    static uint8_t _maxSpeed;
    static uint8_t _minSpeed;
    static uint8_t _speedSteps;
    static uint8_t _startSpeed;
    static Timer _timer;

public:
    static Direction direction();
    static void request(MotorCntl motorCntl, uint8_t speed = 0);
    static void setArduino(SerialIO *arduinoIO);
    static uint8_t currentSpeed();
    static void setSpeeds(uint8_t maxSpeed, uint8_t minSpeed, uint8_t speedSteps, uint8_t startSpeed);
};


#endif //ROBLUCKSPI_CONTROLMOTOR_H
