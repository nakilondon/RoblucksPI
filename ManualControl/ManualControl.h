//
// Created by james on 04/03/19.
//

#ifndef ROBLUCKSPI_MANUALCONTROL_H
#define ROBLUCKSPI_MANUALCONTROL_H

#include "../Joystick/RoblucksJoystick.h"
#include "../Joystick/Joystick.h"
#include "../SerialIO/SerialIO.h"
#include "../Message.h"
#include "../ControlServoMotor/ControlServoMotor.h"


class ManualControl {

private:

public:
    void joystickCommand(JoystickType joystickType, int movement, ControlServoMotor &controlServoMotor);


};


#endif //ROBLUCKSPI_MANUALCONTROL_H
