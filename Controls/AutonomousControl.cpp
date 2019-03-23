//
// Created by james on 04/03/19.
//

#include "AutonomousControl.h"
#include <chrono>
#include <cstdint>
#include <inttypes.h>

std::list<uint8_t > AutonomousControl::_leftDistances;
std::list<uint8_t > AutonomousControl::_rightDistances;
std::list<uint8_t> AutonomousControl::_frontDistances;
uint8_t AutonomousControl::_stopingCnt;

void AutonomousControl::setup(json autoConfig) {
    Log::logMessage(PI, LOG_INFO, "Auto config = " + autoConfig.dump());

    ControlMotor::setSpeeds(
            autoConfig["maxSpeed"].get<uint8_t>(),
            autoConfig["minSpeed"].get<uint8_t>(),
            autoConfig["speedSteps"].get<uint8_t>(),
            autoConfig["startSpeed"].get<uint8_t>());

    auto turnType = autoConfig["turnType"].get<std::string>();

    if (turnType=="normal") {
        AutonomousControl::setTurnType(TURN_NORMAL);
    } else
        AutonomousControl::setTurnType(TURN_3);
}

void AutonomousControl::joystickCommand(JoystickType joystickType) {

    switch (joystickType){
        case joyStop:{
            ControlMotor::request(MOTOR_STOP);
            break;
        }
        case joyStart:{
            ControlServo::request(SERVO_CENTER);
            ControlMotor::request(MOTOR_START);
            _stopingCnt = 0;
            break;
        }
        default:{
            break;
        }
    }
}

void AutonomousControl::processDistance(DistanceSensor sensor, uint8_t measure) {

    if (ControlMotor::direction() != D_FORWARD &&
        ControlMotor::direction() != D_BACK)
        return;

    if (sensor == SENSOR_LEFT || sensor == SENSOR_RIGHT ) {
        uint8_t leftDistance;
        uint8_t rightDistance;
        if (sensor == SENSOR_LEFT) {
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

//        if (ControlServo::processingServoRqst() == SERVO_ADJUST_LEFT ||
//            ControlServo::processingServoRqst() == SERVO_ADJUST_RIGHT)

        if (checkForTurn(rightDistance, leftDistance))
            return;

        if (checkForAdjustment(rightDistance, leftDistance))
            return;

        if (checkForMiddle(rightDistance, leftDistance))
            return;
    }

    if (SENSOR_FRONT && measure > 0) {
        _frontDistances.push_front(measure);
        if (_frontDistances.size()>10)
            _frontDistances.pop_back();
        if (measure <= 5) {
            _stopingCnt++;
            Log::logMessage(PI, LOG_DEBUG, "Stopping distance: " + std::string(measure, sizeof(measure)));
            ControlMotor::request(MOTOR_STOP);
        } else
            _stopingCnt = 0;
    }
}

bool AutonomousControl::checkForTurn(int rightDistance, int leftDistance) {

//    fprintf(stdout, "right array size %d, left arrary size %d, right %d, left %d\n",
     //       _rightDistances.size(), _leftDistances.size(), rightDistance, leftDistance);

 //   if (_rightDistances.size() > 5 && _leftDistances.size() > 5) {

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

        double rightAverage = totalRight / _rightDistances.size();
        double leftAverage = totalLeft / _leftDistances.size();
        double totalLength = rightDistance + leftDistance;
        if (totalLength<=0)
            return false;

        double rightDiff = abs(rightDistance-rightAverage);
        double leftDiff = abs(leftDistance - leftAverage);

        //if ((totalLength > (rightAverage + leftAverage) * 1.5 ) ||
        //     frontDistance < 25){
        if (rightDiff/rightAverage > 0.2 || leftDiff/leftAverage > 0.2) {
           // fprintf(stdout, "total len: %d, right Average: %d, leftAverage %d, front %d\n", totalLength, rightAverage,
           //         leftAverage, frontDistance);
            if ((rightDistance-rightAverage) > (leftDistance-leftAverage))
                ControlServo::request(SERVO_3_RIGHT);
            else
                ControlServo::request(SERVO_3_LEFT);
            return true;
        }

   // }
    return false;
}

bool AutonomousControl::checkForAdjustment(int rightDistance, int leftDistance) {

    double totalLength = rightDistance + leftDistance;
    double difference = abs(rightDistance - leftDistance);
    double percentDifference = difference / totalLength;

    if (percentDifference > 0.3) {
        if ((ControlMotor::direction() == D_FORWARD && rightDistance < leftDistance) ||
            ControlMotor::direction() == D_BACK && rightDistance > leftDistance) {
          //  _leftDistances.pop_front();
            ControlServo::request(SERVO_ADJUST_LEFT);
        } else {
          //  _rightDistances.pop_front();
            ControlServo::request(SERVO_ADJUST_RIGHT);
        }
        return true;
    }
    return false;
}

bool AutonomousControl::checkForMiddle(int rightDistance, int leftDistance) {

    if (ControlServo::processingServoRqst() == SERVO_NONE)
        return false;

    double totalLength = rightDistance + leftDistance;
    double difference = abs(rightDistance - leftDistance);
    double percentDifference = difference / totalLength;

    if (percentDifference < 0.2) {
        ControlServo::request(SERVO_CENTER);
        return true;
    }

    return false;
}

TurnType AutonomousControl::_turnType = TURN_3;

void AutonomousControl::setTurnType(TurnType turnType) {
    _turnType = turnType;
}