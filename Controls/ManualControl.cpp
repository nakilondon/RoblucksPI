//
// Created by james on 04/03/19.
//

#include "ManualControl.h"
void ManualControl::setup(json manualConfig) {
    Log::logMessage(PI, LOG_INFO, "Manual config = " + manualConfig.dump());

    ControlMotor::setSpeeds(
            manualConfig["maxSpeed"].get<uint8_t>(),
            manualConfig["minSpeed"].get<uint8_t>(),
            manualConfig["speedSteps"].get<uint8_t>(),
            manualConfig["startSpeed"].get<uint8_t>());
}

void ManualControl::joystickCommand(JoystickType joystickType, int movement) {
// Attempt to sample an event from the joystick

    switch (joystickType) {
        case joyStop: {
            ControlMotor::request(MOTOR_STOP);
            break;
        }

        case joyIncreaseSpeed: {
            ControlMotor::request(MOTOR_INCREASE_SPEED);
            break;
        }

        case joyDecreaseSpeed: {
            ControlMotor::request(MOTOR_DECREASE_SPEED);
            break;
        }

        case joyMoveBack: {
            ControlMotor::request(MOTOR_MOVE_BACK);
            break;
        }

        case joyMoveForward: {
            ControlMotor::request(MOTOR_MOVE_FORWARD);
            break;
        }

        case joyCenter: {
            ControlServo::request(SERVO_CENTER);
            break;
        };

        case joyMoveLeft: {
            ControlServo::request(SERVO_LEFT, movement);
            break;
        }

        case joyMoveRight: {
            ControlServo::request(SERVO_RIGHT, movement);
            break;
        }

        case joy3Left: {
            ControlServo::request(SERVO_3_LEFT);
            break;
        }

        case joy3Right: {
            ControlServo::request(SERVO_3_RIGHT);
            break;
        }

        default: {
            break;
        }
    }
}
