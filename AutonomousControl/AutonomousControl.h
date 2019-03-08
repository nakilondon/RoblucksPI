//
// Created by james on 04/03/19.
//

#ifndef ROBLUCKSPI_AUTONOMOUSCONTROL_H
#define ROBLUCKSPI_AUTONOMOUSCONTROL_H

#include "../Message.h"
#include "../parameters.h"
#include "../Joystick/Joystick.h"
#include "../Joystick/RoblucksJoystick.h"
#include "../SerialIO/SerialIO.h"
#include "../ControlServoMotor/ControlServoMotor.h"
#include <list>
#include "../Timer.h"

enum Adjustment {
    NONE,
    ADJUST_LEFT,
    ADJUST_RIGHT,
    TURNING_LEFT,
    TURNING_RIGHT
};

class AutonomousControl {

private:
    std::list<uint8_t > _leftDistances;
    std::list<uint8_t > _rightDistances;
    std::list<uint8_t> _frontDistances;
    uint8_t _stopingCnt = 0;
    bool checkForTurn(ControlServoMotor &controlServoMotor, int totalLength, int rightDistance, int leftDistance);

public:
    void joystickCommand(JoystickType joystickType, ControlServoMotor &controlServoMotor);
    void processDistance(Direction direction, uint8_t measure, ControlServoMotor &controlServoMotor, Mode *currentMode);
};


#endif //ROBLUCKSPI_AUTONOMOUSCONTROL_H
