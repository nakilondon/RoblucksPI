//
// Created by james on 04/03/19.
//

#include "AutonomousControl.h"
#include <chrono>
#include <cstdint>
#include <inttypes.h>

std::list<short> AutonomousControl::_leftDistances;
std::list<short> AutonomousControl::_rightDistances;
std::list<short> AutonomousControl::_frontDistances;
uint8_t AutonomousControl::_stopingCnt;
bool AutonomousControl::_turningSpeedSet = false;
uint8_t  AutonomousControl::_turningSpeed = 1;
uint8_t  AutonomousControl::_straightSpeed = 3;
Timer AutonomousControl::_timer;
bool AutonomousControl::_stopped = true;
uint8_t AutonomousControl::_turnTime = 30;

void AutonomousControl::setup(json autoConfig) {
    Log::logMessage(PI, LOG_INFO, "Auto config = " + autoConfig.dump());

    ControlMotor::setSpeeds(
            autoConfig["startSpeed"].get<uint8_t>(),
            autoConfig["minSpeed"].get<uint8_t>(),
            autoConfig["speedSteps"].get<uint8_t>(),
            autoConfig["startSpeed"].get<uint8_t>());

    _turningSpeed = autoConfig["turnSpeed"].get<uint8_t>();
    _straightSpeed = autoConfig["straightSpeed"].get<uint8_t>();
    _turnTime = autoConfig["turnTime"].get<uint8_t >();

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
            _stopped = true;
            break;
        }
        case joyStart:{
            _frontDistances.empty();
            _leftDistances.empty();
            _rightDistances.empty();
            ControlServo::request(SERVO_CENTER);
            usleep(1000);
            ControlMotor::request(MOTOR_START, _straightSpeed);
            _stopped = false;
            break;
        }
        default:{
            break;
        }
    }
}

void AutonomousControl::processDistance(DistanceSensor sensor, short measure) {

    if (ControlMotor::direction() != D_FORWARD &&
        ControlMotor::direction() != D_BACK)
        return;

    if (ControlServo::processingServoRqst() != SERVO_NONE &&
        ControlServo::processingServoRqst() != SERVO_ADJUST_LEFT &&
        ControlServo::processingServoRqst() != SERVO_ADJUST_RIGHT)
        return;

    if (_stopped)
        return;

    if (sensor == SENSOR_LEFT || sensor == SENSOR_RIGHT ) {

        short leftDistance;
        short rightDistance;
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

        if (checkForAdjustment(sensor, measure))
            return;

  //      if (checkForMiddle(rightDistance, leftDistance))
  //          return;
    }

    if (sensor == SENSOR_FRONT && measure > 0) {
   //     Log::logMessage(PI, LOG_DEBUG, "Front Stopping distance: " + std::to_string(measure));
        _frontDistances.push_front(measure);
        if (_frontDistances.size()>10)
            _frontDistances.pop_back();
        if (measure <= 11) {
            _stopingCnt++;
            Log::logMessage(PI, LOG_DEBUG, "Stopping distance: " + std::to_string(measure));
            ControlMotor::request(MOTOR_MOVE_BACK, _turningSpeed);
            if (_leftDistances.front() > _rightDistances.front())
                ControlServo::request(SERVO_RIGHT, 50);
            else
                ControlServo::request(SERVO_LEFT,50);

            _timer.setTimeout([&]() {
                Log::logMessage(PI, LOG_DEBUG, "Servo turn left timer timed out");
                ControlMotor::request(MOTOR_MOVE_FORWARD, _turningSpeed);
                ControlServo::request(SERVO_CENTER);
                _timer.stop();
                //}, 1500 - ControlMotor::currentSpeed() * 10);
            }, 1000);

        }

        if (measure<70 && !_turningSpeedSet) {
            _turningSpeedSet = true;
            Log::logMessage(PI, LOG_DEBUG, "Setting turning speed, distance = " + std::to_string(measure));
            ControlMotor::request(MOTOR_MOVE_FORWARD, _turningSpeed);
            }
        else if (measure >= 70 && _turningSpeed){
            _turningSpeedSet = false;
            Log::logMessage(PI, LOG_DEBUG, "Setting straight speed, distance = " + std::to_string(measure));
            ControlMotor::request(MOTOR_MOVE_FORWARD, _straightSpeed);
        }
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
        std::list<short>::iterator it;

        for (it = _rightDistances.begin(); it != _rightDistances.end(); it++)
            totalRight += *it;

        for (it = _leftDistances.begin(); it != _leftDistances.end(); it++)
            totalLeft += *it;

        double rightAverage = totalRight / _rightDistances.size();
        double leftAverage = totalLeft / _leftDistances.size();
        double totalLength = rightDistance + leftDistance;

        if (totalLength<=0)
            return false;

        double rightDiff = rightDistance-rightAverage;
        double leftDiff = leftDistance - leftAverage;

        //if ((totalLength > (rightAverage + leftAverage) * 1.5 ) ||
        //     frontDistance < 25){
        if (((abs(rightDiff)/rightAverage) > 0.3 || (abs(leftDiff)/leftAverage) > 0.3) && frontDistance < 60) {
           // fprintf(stdout, "total len: %d, right Average: %d, leftAverage %d, front %d\n", totalLength, rightAverage,
           //         leftAverage, frontDistance);
           Log::logMessage(PI, LOG_INFO  , "Turing left average:  " +
              std::to_string(leftAverage) + " distance: " + std::to_string(leftDistance) +
              "  right average:  " +     std::to_string(rightAverage) + " distance: " + std::to_string(rightDistance));

         //  if (rightDiff > leftDiff) {
         if (rightDistance > leftDistance){
                if (_turnType == TURN_NORMAL) {
                    ControlServo::request(SERVO_TURN_RIGHT, _turnTime);
                } else {
                    ControlServo::request(SERVO_3_RIGHT);
                }
            } else {
                if (_turnType== TURN_NORMAL) {
                    ControlServo::request(SERVO_TURN_LEFT, _turnTime);
                } else {
                    ControlServo::request(SERVO_3_LEFT);
                }
            }
            return true;
        }

   // }
    return false;
}

bool AutonomousControl::checkForAdjustment(DistanceSensor sensor, short measure) {


    if (measure <= 10 && measure > 0) {
        if (sensor==SENSOR_LEFT){
            Log::logMessage(PI, LOG_DEBUG, "Adjust Right left sensor measure = " + std::to_string(measure));
            ControlServo::request(SERVO_ADJUST_RIGHT);
        } else {
            Log::logMessage(PI, LOG_DEBUG, "Adjust Left right sensor measure = " + std::to_string(measure));
            ControlServo::request(SERVO_ADJUST_LEFT);
        }
        return true;
    }
/*    double totalLength = rightDistance + leftDistance;
    double difference = abs(rightDistance - leftDistance);
    double percentDifference = difference / totalLength;

    if (percentDifference > 0.2) {
        if ((ControlMotor::direction() == D_FORWARD && rightDistance < leftDistance) ||
            ControlMotor::direction() == D_BACK && rightDistance > leftDistance) {
          //  _leftDistances.pop_front();
            ControlServo::request(SERVO_ADJUST_LEFT);
        } else {
          //  _rightDistances.pop_front();
            ControlServo::request(SERVO_ADJUST_RIGHT);
        }
        return true;
    } */



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

TurnType AutonomousControl::_turnType = TURN_NORMAL;

void AutonomousControl::setTurnType(TurnType turnType) {
    _turnType = turnType;
}