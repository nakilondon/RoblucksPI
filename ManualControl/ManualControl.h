//
// Created by james on 04/03/19.
//

#ifndef ROBLUCKSPI_MANUALCONTROL_H
#define ROBLUCKSPI_MANUALCONTROL_H

#include "../Joystick/RoblucksJoystick.h"
#include "../Joystick/Joystick.h"
#include "../SerialIO/SerialIO.h"
#include "../Message.h"


class ManualControl {

private:
    //static uint8_t _currentSpeed;
    //static MotorCmd _currentMotorCmd;

public:
    static void joystickCommand(JoystickType joystickType, int movement, SerialIO *ardunioIO, uint8_t &currentSpeed, MotorCmd &currentMotorCmd);

};


#endif //ROBLUCKSPI_MANUALCONTROL_H
