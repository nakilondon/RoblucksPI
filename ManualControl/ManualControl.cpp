//
// Created by james on 04/03/19.
//

#include "ManualControl.h"
#include "../parameters.h"

void ManualControl::joystickCommand(JoystickType joystickType, int movement, ControlServoMotor &controlServoMotor) {
// Attempt to sample an event from the joystick

    switch (joystickType) {
        case joyStop: {
            controlServoMotor.request(cntlStop, 0);
            break;
        }

        case joyIncreaseSpeed: {
            controlServoMotor.request(cntlIncreaseSpeed, 0);
            break;
        }

        case joyDecreaseSpeed: {
            controlServoMotor.request(cntlDecreaseSpeed, 0);
            break;
        }

        case joyMoveBack: {
            controlServoMotor.request(cntlMoveBack, 0);
            break;
        }

        case joyMoveForward: {
            controlServoMotor.request(cntlMoveForward, 0);
            break;
        }

        case joyCenter: {
            controlServoMotor.request(cntlCenter, 0);
            break;
        };

        case joyMoveLeft: {
            controlServoMotor.request(cntlLeft, movement);
            break;
        }

        case joyMoveRight: {
            controlServoMotor.request(cntlRight, movement);
            break;
        }

        default: {
            break;
        }
    }
}
