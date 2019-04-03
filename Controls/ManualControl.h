//
// Created by james on 04/03/19.
//

#ifndef ROBLUCKSPI_MANUALCONTROL_H
#define ROBLUCKSPI_MANUALCONTROL_H

#include "../Joystick/RoblucksJoystick.h"
#include "../Joystick/Joystick.h"
#include "../SerialIO/SerialIO.h"
#include "../Message.h"
#include "ControlServo.h"
#include "ControlMotor.h"
#include "../parameters.h"
#include "../Utils/json.hpp"

using json = nlohmann::json;

class ManualControl {

private:

public:
    static void setup(json manualConfig);
    static void joystickCommand(JoystickType joystickType, int movement);


};


#endif //ROBLUCKSPI_MANUALCONTROL_H
