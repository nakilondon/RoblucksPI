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
#include <list>

enum Adjustment {
    NONE,
    ADJUST_LEFT,
    ADJUST_RIGHT,
    TURNING_LEFT,
    TURNING_RIGHT
};

class AutonomousControl {
    /*
private:
    static Adjustment _adjustment;
    static std::list<uint8_t > _leftDistances;
    static std::list<uint8_t > _rightDistances;
*/
public:
    static void joystickCommand(JoystickType joystickType, int movement, SerialIO *ardunioIO, uint8_t &currentSpeed, MotorCmd &currentMotorCmd);
    static void processDistance(SerialIO *ardunioIO, uint8_t &currentSpeed, MotorCmd &currentMotorCmd, Adjustment &adjustment,
            std::list<uint8_t> &leftDistances, std::list<uint8_t> &rightDistances);


};


#endif //ROBLUCKSPI_AUTONOMOUSCONTROL_H
