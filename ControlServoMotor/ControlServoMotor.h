//
// Created by james on 06/03/19.
//

#ifndef ROBLUCKSPI_CONTROLSERVOMOTOR_H
#define ROBLUCKSPI_CONTROLSERVOMOTOR_H

#include "../Message.h"
#include "../parameters.h"
#include "../SerialIO/SerialIO.h"
#include <unistd.h>
#include <cstdint>
#include "../Timer.h"
#include <chrono>
#include <cstdint>
#include <inttypes.h>

enum ControlType {
    cntlNone,
    cntlTurnLeft,
    cntlTurnRight,
    cntlCenter,
    cntlMoveForward,
    cntlMoveBack,
    cntlStop,
    cntlStart,
    cntlIncreaseSpeed,
    cntlDecreaseSpeed,
    cntl3Left,
    cntl3Right,
    cntlLeft,
    cntlRight,
    cntlAdjustLeft,
    cntlAdjustRight
};

enum ServoCntl {
    servoNone,
    servoAdjustLeft,
    servoAdjustRight,
    servoTurnLeft,
    servoTurnRight
};

class ControlServoMotor {

private:
    MotorCmd _currentMotorCmd;
    uint8_t _currentSpeed;
    ServoCntl _servoCntl = servoNone;
    Timer _timer;
    SerialIO _arduinoIO = SerialIO("", 115200);

public:
    MotorCmd direction();
    ServoCntl processingServoRqst();
    void request(ControlType cntlType, uint8_t movement = 0);
    void setArduino(SerialIO *arduinoIO);
};


#endif //ROBLUCKSPI_CONTROLSERVOMOTOR_H
