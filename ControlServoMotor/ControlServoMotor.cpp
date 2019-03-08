//
// Created by james on 06/03/19.
//

#include "ControlServoMotor.h"

void ControlServoMotor::request(ControlType cntlType, uint8_t movement) {

    switch (cntlType) {
        case (cntlStop): {
            char msgToSend[] = {MOTOR, STOP};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            _currentMotorCmd = STOP;
            break;
        }
        case (cntlStart): {
            fprintf(stdout, "start \n");
            _currentSpeed = START_SPEED;
            _currentMotorCmd = FORWARD;
            char msgToSend[] = {MOTOR, static_cast<char>(_currentMotorCmd), _currentSpeed};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            break;
        }
        case (cntlLeft): {
            fprintf(stdout, "left \n");
            char msgToSend[] = {SERVO, LEFT, static_cast<char>(movement)};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            break;
        }
        case (cntlRight): {
            fprintf(stdout, "right \n");
            char msgToSend[] = {SERVO, RIGHT, static_cast<char>(movement)};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            break;
        }
        case (cntlCenter): {
            fprintf(stdout, "center \n");
            char msgToSend[] = {SERVO, CENTER};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            break;
        }
        case (cntlTurnLeft): {
            _servoCntl = servoTurnLeft;
            fprintf(stdout, "left turn\n");
            char msgToSend[] = {SERVO, LEFT, 100};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            fprintf(stdout, "turn left set timer \n");
            _timer.setTimeout([&]() {
                fprintf(stdout, "Turn left timeout \n");
                request(cntlCenter);

                _timer.stop();
                _servoCntl = servoNone;
            }, 2000);

            break;
        }

        case (cntlTurnRight): {
            _servoCntl = servoTurnRight;
            fprintf(stdout, "right turn\n");
            char msgToSend[] = {SERVO, RIGHT, 100};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            fprintf(stdout, "turn right set timer \n");
            _timer.setTimeout([&]() {
                fprintf(stdout, "Turn right timeout \n");
                request(cntlCenter);

                _timer.stop();
                _servoCntl = servoNone;
            }, 2000);
            break;
        }

        case (cntl3Left):{

        }
        case (cntl3Right):{

        }
        case (cntlAdjustLeft):{
            if (_servoCntl == servoNone) {
                _servoCntl = servoAdjustLeft;
                char msgToSend[] = {SERVO, LEFT, 50};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
                fprintf(stdout,"Adjust left set timer \n");
                _timer.setTimeout([&]() {
                    fprintf(stdout,"Adjust left timeout \n");
                    if (_servoCntl == servoNone || _servoCntl == servoAdjustLeft) {
                        request(cntlCenter);
                        _servoCntl == servoNone;
                    }
                    _timer.stop();
                }, 200);

            }
            break;

        }
        case (cntlAdjustRight):{
            if (_servoCntl == servoNone) {
                _servoCntl == servoAdjustRight;
                char msgToSend[] = {SERVO, RIGHT, 50};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
                fprintf(stdout,"Adjust right set timer \n");
                _timer.setTimeout([&]() {
                    fprintf(stdout,"Adjust right timeout \n");
                    if (_servoCntl == servoNone || _servoCntl == servoAdjustLeft) {
                        request(cntlCenter);
                        _servoCntl == servoNone;
                    }
                    _timer.stop();
                }, 200);
            }
            break;
        }

        case (cntlMoveForward):{
            if (_currentMotorCmd != FORWARD) {
                _currentSpeed = START_SPEED;
                fprintf(stdout,"Move Forward \n");
                _currentMotorCmd = FORWARD;
                char msgToSend[] = {MOTOR, static_cast<char>(_currentMotorCmd), _currentSpeed};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            }
            break;

        }
        case (cntlMoveBack):{
            if (_currentMotorCmd != REVERSE) {
                fprintf(stdout,"Move Back \n");
                _currentSpeed = START_SPEED;
                _currentMotorCmd = REVERSE;
                char msgToSend[] = {MOTOR, static_cast<char>(_currentMotorCmd), _currentSpeed};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }
        case (cntlIncreaseSpeed):{
            if (_currentSpeed + SPEED_STEPS <= MAX_SPEED) {
                _currentSpeed += SPEED_STEPS;
                char msgToSend[] = {MOTOR, static_cast<char>(_currentMotorCmd), static_cast<char>(_currentSpeed)};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }
        case (cntlDecreaseSpeed):{
            if (_currentSpeed - SPEED_STEPS >= START_SPEED) {
                _currentSpeed -= SPEED_STEPS;
                char msgToSend[] = {MOTOR, static_cast<char>(_currentMotorCmd), static_cast<char>(_currentSpeed)};
                _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            }
            break;
        }
        default:{
            break;
        }
    }
}

MotorCmd ControlServoMotor::direction() {
    return _currentMotorCmd;
}

ServoCntl ControlServoMotor::processingServoRqst() {
    return _servoCntl;
}

void ControlServoMotor::setArduino(SerialIO *arduinoIO) {
    _arduinoIO = *arduinoIO;
}

