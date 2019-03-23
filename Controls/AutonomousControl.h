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
#include "ControlServo.h"
#include "ControlMotor.h"
#include <list>
#include "../Utils/Timer.h"
#include "../Utils/Log.h"
#include "../Utils/json.hpp"

using json = nlohmann::json;

enum TurnType {
    TURN_NORMAL,
    TURN_3
};

class AutonomousControl {

private:
    static TurnType _turnType;
    static std::list<uint8_t > _leftDistances;
    static std::list<uint8_t > _rightDistances;
    static std::list<uint8_t> _frontDistances;
    static uint8_t _stopingCnt;
    static bool checkForTurn(int rightDistance, int leftDistance);
    static bool checkForAdjustment(int rightDistance, int leftDistance);
    static bool checkForMiddle(int rightDistance, int leftDistance);

public:
    static void joystickCommand(JoystickType joystickType);
    static void processDistance(DistanceSensor sensor, uint8_t measure);
    static void setTurnType(TurnType turnType);
    static void setup(json manualConfig);
};


#endif //ROBLUCKSPI_AUTONOMOUSCONTROL_H
