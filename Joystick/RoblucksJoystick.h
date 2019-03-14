//
// Created by james on 04/03/19.
//

#ifndef ROBLUCKSPI_ROBLUCKSJOYSTICK_H
#define ROBLUCKSPI_ROBLUCKSJOYSTICK_H

#include "Joystick.h"

enum JoystickType {
    joyNone,
    joyMoveLeft,
    joyMoveRight,
    joyCenter,
    joyMoveForward,
    joyMoveBack,
    joyStop,
    joyStart,
    joyIncreaseSpeed,
    joyDecreaseSpeed,
    joy3Left,
    joy3Right
};

class RoblucksJoystick {
private:
    static constexpr double _servoScale  = 100 / (double)JoystickEvent::MAX_AXES_VALUE;
    Joystick _joystick;

public:
    JoystickType checkJoystick(int &movement);
    bool isFound();
    void resetJoystick();
};


#endif //ROBLUCKSPI_ROBLUCKSJOYSTICK_H
