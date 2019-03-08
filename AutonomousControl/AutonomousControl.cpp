//
// Created by james on 04/03/19.
//

#include "AutonomousControl.h"
#include <chrono>
#include <cstdint>
#include <inttypes.h>

void AutonomousControl::joystickCommand(JoystickType joystickType, ControlServoMotor &controlServoMotor) {

    switch (joystickType){
        case joyStop:{
            controlServoMotor.request(cntlStop);
            break;
        }
        case joyStart:{
            controlServoMotor.request(cntlCenter);
            controlServoMotor.request(cntlStart);
            _stopingCnt = 0;
            break;
        }
        default:{
            break;
        }
    }
}

void AutonomousControl::processDistance(Direction direction, uint8_t measure, ControlServoMotor &controlServoMotor, Mode *currentMode) {

    if (*currentMode != AUTONOMOUS)
        return;

    if (controlServoMotor.direction() != FORWARD &&
        controlServoMotor.direction() != REVERSE &&
        controlServoMotor.processingServoRqst() != servoTurnRight &&
        controlServoMotor.processingServoRqst() != servoTurnLeft)
        return;

    if (direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT) {
        uint8_t leftDistance;
        uint8_t rightDistance;
        if (direction == DIRECTION_LEFT) {
            _leftDistances.push_front(measure);
            leftDistance = measure;
            if (_rightDistances.empty())
                return;
            rightDistance = _rightDistances.front();

            if (_rightDistances.size() > 10)
                _rightDistances.pop_back();

        } else {
            _rightDistances.push_front(measure);
            rightDistance = measure;
            if (_leftDistances.empty())
                return;
            leftDistance = _leftDistances.front();
            if (_leftDistances.size() > 10)
                _leftDistances.pop_back();
        }
        auto totalLength = rightDistance + leftDistance;

        if (checkForTurn(controlServoMotor, totalLength, rightDistance, leftDistance))
            return;

        auto difference = abs(rightDistance - leftDistance);

        ControlType cntlType;

        if (difference / totalLength > 0.3) {
            if ((controlServoMotor.direction() == FORWARD && rightDistance < leftDistance) ||
                controlServoMotor.direction() == REVERSE && rightDistance > leftDistance) {
                cntlType = cntlAdjustLeft;
                _leftDistances.pop_front();
            } else {
                cntlType = cntlAdjustRight;
                _rightDistances.pop_front();
            }
            controlServoMotor.request(cntlType);

        } else if (difference <= 0.2) {
            controlServoMotor.request(cntlCenter);
        }
    }

    if (DIRECTION_FRONT && measure > 0) {
        _frontDistances.push_front(measure);
        if (_frontDistances.size()>10)
            _frontDistances.pop_back();
        if (measure <= 8) {
            _stopingCnt++;
            fprintf(stdout, "Stopping distance: %d, count: %d\n", measure, _stopingCnt);
          //  if(_stopingCnt>1)
                controlServoMotor.request(cntlStop);
        } else
            _stopingCnt = 0;
    }
}

bool AutonomousControl::checkForTurn(ControlServoMotor &controlServoMotor, int totalLength, int rightDistance, int leftDistance) {
    if (_rightDistances.size() > 5 && _leftDistances.size() > 5) {

        int frontDistance = 200;
        if (!_frontDistances.empty())
            frontDistance = _frontDistances.front();

        int totalRight = 0;
        int totalLeft = 0;
        std::list<uint8_t>::iterator it;

        for (it = _rightDistances.begin(); it != _rightDistances.end(); it++)
            totalRight += *it;

        for (it = _leftDistances.begin(); it != _leftDistances.end(); it++)
            totalLeft += *it;

        auto rightAverage = totalRight / _rightDistances.size();
        auto leftAverage = totalLeft / _leftDistances.size();

        if (totalLength > (rightAverage + leftAverage) * 1.4 ) {
            fprintf(stdout, "total len: %d, right Average: %d, leftAverage %d, front %d\n", totalLength, rightAverage,
                    leftAverage, frontDistance);
            if (rightDistance > leftDistance)
                controlServoMotor.request(cntlTurnRight);
            else
                controlServoMotor.request(cntlTurnLeft);
            return true;
        }
    }
    return false;
}