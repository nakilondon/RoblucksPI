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
    static std::list<short> _leftDistances;
    static std::list<short> _rightDistances;
    static std::list<short> _frontDistances;
    static uint8_t _stopingCnt;
    static bool checkForTurn(int rightDistance, int leftDistance);
    static bool checkForAdjustment(DistanceSensor sensor, short measure);
    static bool checkForMiddle(int rightDistance, int leftDistance);
    static bool _turningSpeedSet;
    static uint8_t _straightSpeed;
    static uint8_t _turningSpeed;
    static Timer _timer;
    static bool _stopped;
    static uint8_t _turnTime;

public:
    static void joystickCommand(JoystickType joystickType);
    static void processDistance(DistanceSensor sensor, short measure);
    static void setTurnType(TurnType turnType);
    static void setup(json manualConfig);
};


#endif //ROBLUCKSPI_AUTONOMOUSCONTROL_H
