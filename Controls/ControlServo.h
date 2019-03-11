//
// Created by james on 06/03/19.
//

#ifndef ROBLUCKSPI_CONTROLSERVO_H
#define ROBLUCKSPI_CONTROLSERVO_H

#include "../Message.h"
#include "../parameters.h"
#include "../SerialIO/SerialIO.h"
#include "ControlMotor.h"
#include <unistd.h>
#include <cstdint>
#include "../Utils/Timer.h"
#include "../Utils/Log.h"
#include <chrono>
#include <cstdint>
#include <inttypes.h>

enum ServoCntl {
    SERVO_NONE,
    SERVO_TURN_LEFT,
    SERVO_TURN_RIGHT,
    SERVO_CENTER,
    SERVO_3_LEFT,
    SERVO_3_RIGHT,
    SERVO_LEFT,
    SERVO_RIGHT,
    SERVO_ADJUST_LEFT,
    SERVO_ADJUST_RIGHT
};

class ControlServo {

private:
    static ServoCntl _servoCntl;
    static Timer _timer;
    static SerialIO _arduinoIO;

public:
    static ServoCntl processingServoRqst();
    static void request(ServoCntl cntlType, uint8_t movement = 0);
    static void setArduino(SerialIO *arduinoIO);
};


#endif //ROBLUCKSPI_CONTROLSERVO_H
