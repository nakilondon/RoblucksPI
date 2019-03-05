//
// Created by james on 04/03/19.
//

#include "ManualControl.h"
#include "../parameters.h"

void ManualControl::joystickCommand(JoystickType joystickType, int movement, SerialIO *ardunioIO, uint8_t &currentSpeed, MotorCmd &currentMotorCmd) {
// Attempt to sample an event from the joystick

    switch (joystickType) {
        case joyStop: {
            char msgToSend[] = {MOTOR, STOP};
            ardunioIO->Send(msgToSend, sizeof(msgToSend));
            currentMotorCmd = STOP;
            break;
        }

        case joyIncreaseSpeed: {
            if (currentSpeed + SPEED_STEPS <= MAX_SPEED) {
                currentSpeed += SPEED_STEPS;
                char msgToSend[] = {MOTOR, static_cast<char>(currentMotorCmd), static_cast<char>(currentSpeed)};
                ardunioIO->Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }

        case joyDecreaseSpeed: {
            if (currentSpeed - SPEED_STEPS >= START_SPEED) {
                currentSpeed -= SPEED_STEPS;
                char msgToSend[] = {MOTOR, static_cast<char>(currentMotorCmd), static_cast<char>(currentSpeed)};
                ardunioIO->Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }

        case joyMoveBack: {
            if (currentMotorCmd != REVERSE) {
                currentSpeed = START_SPEED;
                currentMotorCmd = REVERSE;
                char msgToSend[] = {MOTOR, static_cast<char>(currentMotorCmd), currentSpeed};
                ardunioIO->Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }

        case joyMoveForward: {
            if (currentMotorCmd != FORWARD) {
                currentSpeed = START_SPEED;
                currentMotorCmd = FORWARD;
                char msgToSend[] = {MOTOR, static_cast<char>(currentMotorCmd), currentSpeed};
                ardunioIO->Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }

        case joyCenter: {
            char msgToSend[] = {SERVO, CENTER};
            ardunioIO->Send(msgToSend, sizeof(msgToSend));
            break;
        };

        case joyMoveLeft: {
            char msgToSend[] = {SERVO, LEFT, static_cast<char>(movement)};
            ardunioIO->Send(msgToSend, sizeof(msgToSend));
            break;
        }

        case joyMoveRight: {
            char msgToSend[] = {SERVO, RIGHT, static_cast<char>(movement)};
            ardunioIO->Send(msgToSend, sizeof(msgToSend));
            break;
        }

        default: {
            break;
        }
    }
}
