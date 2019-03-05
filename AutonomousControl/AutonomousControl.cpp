//
// Created by james on 04/03/19.
//

#include "AutonomousControl.h"

void AutonomousControl::joystickCommand(JoystickType joystickType, int movement, SerialIO *ardunioIO,
                                        uint8_t &currentSpeed, MotorCmd &currentMotorCmd) {

    switch (joystickType){
        case joyStop:{
            char msgToSend[] = {MOTOR, STOP};
            ardunioIO->Send(msgToSend, sizeof(msgToSend));
            currentSpeed = 0;
            currentMotorCmd = STOP;
            break;
        }
        case joyStart:{
            char msgToSend[] = {MOTOR, FORWARD, START_SPEED};
            ardunioIO->Send(msgToSend, sizeof(msgToSend));
            currentMotorCmd = FORWARD;
            currentSpeed = START_SPEED;
            break;
        }
        default:{
            break;
        }
    }
}

void AutonomousControl::processDistance(SerialIO *ardunioIO, uint8_t &currentSpeed, MotorCmd &currentMotorCmd,
        Adjustment &adjustment, std::list<uint8_t> &leftDistances, std::list<uint8_t> &rightDistances) {

    Direction direction = static_cast<Direction >(ardunioIO->Read());
    uint8_t measure = ardunioIO->Read();

    if (direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT) {
        uint8_t leftDistance;
        uint8_t rightDistance;
        if (direction == DIRECTION_LEFT) {
            leftDistances.push_front(measure);
            leftDistance = measure;
            if (rightDistances.empty())
                return;
            rightDistance = rightDistances.front();
            rightDistances.pop_back();

        } else {
            rightDistances.push_front(measure);
            rightDistance = measure;
            if (leftDistances.empty())
                return;
            leftDistance = leftDistances.front();
            leftDistances.pop_back();
        }

        auto totalLength = rightDistance + leftDistance;
        auto difference = abs(rightDistance - leftDistance);
        auto middel = totalLength / 2;

        switch (adjustment) {
            case NONE: {
                if (difference >= middel * 0.2) {
                    char messageToSend[3] = {SERVO};
                    if ((currentMotorCmd == FORWARD && rightDistance < leftDistance) ||
                        currentMotorCmd == REVERSE && rightDistance > leftDistance) {
                        messageToSend[1] = LEFT;
                        adjustment = ADJUST_LEFT;
//    double servoDistance = abs(middel - leftDistance);
//    servoDistance = servoDistance / middel;
//    servoDistance = servoDistance * 100;
//    messageToSend[2] = static_cast<uint8_t>(servoDistance);
                    } else {
                        messageToSend[1] = RIGHT;
                        adjustment = ADJUST_RIGHT;
//  double servoDistance = abs(middel - rightDistance);
//  servoDistance = servoDistance / middel;
//  servoDistance = servoDistance * 100;
//  messageToSend[2] = static_cast<uint8_t>(servoDistance);
                    }
                    messageToSend[2] = 50;
                    ardunioIO->Send(messageToSend, sizeof(messageToSend));
                }
                break;
            }

            case ADJUST_LEFT:
            case ADJUST_RIGHT: {
                if (difference <= middel * .1) {
                    char messageToSend[] = {SERVO, CENTER};
                    ardunioIO->Send(messageToSend, sizeof(messageToSend));
                    adjustment = NONE;
                }
                break;
            }
        }
    }

    if (DIRECTION_FRONT && measure <= 5 && measure > 0) {
        char messageToSend[] = {MOTOR, STOP};
        ardunioIO->Send(messageToSend, sizeof(messageToSend));
    }
}