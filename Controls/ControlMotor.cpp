//
// Created by james on 06/03/19.
//

#include "ControlMotor.h"

Direction ControlMotor::_currentDirection = D_STOP;
uint8_t ControlMotor::_currentSpeed;
SerialIO ControlMotor::_arduinoIO = SerialIO("", 115200);


void ControlMotor::request(MotorCntl motorCntl, uint8_t speed) {

    switch (motorCntl) {
        case (MOTOR_STOP): {
            Log::logMessage(PI, LOG_DEBUG, "Motor request stop");
            char msgToSend[] = {MOTOR, STOP};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            _currentDirection = D_STOP;
            break;
        }
        case (MOTOR_START): {
            Log::logMessage(PI, LOG_DEBUG, "Motor request start");
            request(MOTOR_MOVE_FORWARD);
            break;
        }

        case (MOTOR_MOVE_FORWARD):{
            Log::logMessage(PI, LOG_DEBUG, "Motor request move forward");
            if (_currentDirection != D_FORWARD) {
                _currentSpeed = _startSpeed;
                char msgToSend[] = {MOTOR, static_cast<char>(FORWARD), static_cast<char>(_currentSpeed)};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
                _currentDirection = D_FORWARD;
            }
            break;

        }
        case (MOTOR_MOVE_BACK):{
            if (_currentDirection != D_BACK) {
                Log::logMessage(PI, LOG_DEBUG, "Motor request move back");
                _currentSpeed = _startSpeed;
                char msgToSend[] = {MOTOR, static_cast<char>(REVERSE), static_cast<char>(_currentSpeed)};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
                _currentDirection = D_BACK;
            }
            break;
        }
        case (MOTOR_INCREASE_SPEED):{
            Log::logMessage(PI, LOG_DEBUG, "Motor request increase speed");
            if (_currentSpeed + _speedSteps <= _maxSpeed) {
                _currentSpeed += _speedSteps;
                char msgToSend[] = {MOTOR, 0, static_cast<char>(_currentSpeed)};
                if (direction() == D_FORWARD) {
                    msgToSend[1] = static_cast<char>(FORWARD);
                } else if (direction()== D_BACK){
                    msgToSend[1] = static_cast<char>(REVERSE);
                } else
                    return;

                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }
        case (MOTOR_DECREASE_SPEED):{
            Log::logMessage(PI, LOG_DEBUG, "Motor request descrease speed");
            if (_currentSpeed - _speedSteps >= _minSpeed) {
                _currentSpeed -= _speedSteps;
                char msgToSend[] = {MOTOR, 0, static_cast<char>(_currentSpeed)};
                if (direction() == D_FORWARD) {
                    msgToSend[1] = static_cast<char>(FORWARD);
                } else if (direction()== D_BACK){
                    msgToSend[1] = static_cast<char>(REVERSE);
                } else
                    return;
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }
        default:{
            break;
        }
    }
}

void ControlMotor::setArduino(SerialIO *arduinoIO) {
    _arduinoIO = *arduinoIO;
}

Direction ControlMotor::direction() {
    return _currentDirection;
}

uint8_t ControlMotor::currentSpeed() {
    return _currentSpeed;
}

void ControlMotor::setSpeeds(uint8_t maxSpeed, uint8_t minSpeed, uint8_t speedSteps, uint8_t startSpeed) {
    _maxSpeed = maxSpeed;
    _minSpeed = minSpeed;
    _speedSteps = speedSteps;
    _startSpeed = startSpeed;
}

uint8_t ControlMotor::_startSpeed = 10;
uint8_t ControlMotor::_speedSteps = 5;
uint8_t ControlMotor::_maxSpeed = 40;
uint8_t ControlMotor::_minSpeed = 5;