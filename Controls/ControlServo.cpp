//
// Created by james on 06/03/19.
//

#include "ControlServo.h"

ServoCntl ControlServo::_servoCntl = SERVO_NONE;
Timer ControlServo::_timer;
SerialIO ControlServo::_arduinoIO = SerialIO("", 115200);

void ControlServo::request(ServoCntl cntlType, uint8_t movement) {

    switch (cntlType) {
        case (SERVO_LEFT): {
            Log::logMessage(PI, LOG_DEBUG, "Servo request left");
            char msgToSend[] = {SERVO, LEFT, static_cast<char>(movement)};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            break;
        }

        case (SERVO_RIGHT): {
            Log::logMessage(PI, LOG_DEBUG, "Servo request right");
            char msgToSend[] = {SERVO, RIGHT, static_cast<char>(movement)};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            break;
        }

        case (SERVO_CENTER): {
            Log::logMessage(PI, LOG_DEBUG, "Servo request center");
            char msgToSend[] = {SERVO, CENTER};
            _arduinoIO.Send(msgToSend, sizeof(msgToSend));
            _servoCntl = SERVO_NONE;
            break;
        }

        case (SERVO_TURN_LEFT): {
            Log::logMessage(PI, LOG_DEBUG, "Servo request turn left");
            if (_servoCntl == SERVO_NONE ||
                _servoCntl == SERVO_ADJUST_RIGHT ||
                _servoCntl == SERVO_ADJUST_LEFT) {
                _servoCntl = SERVO_TURN_LEFT;
                if (movement=0)
                    movement = 30;

                request(SERVO_LEFT, 100);
                Log::logMessage(PI, LOG_DEBUG, "Servo turn left timer set");
                _timer.setTimeout([&]() {
                    Log::logMessage(PI, LOG_DEBUG, "Servo turn left timer timed out");
                    request(SERVO_CENTER);

                    _timer.stop();
                    _servoCntl = SERVO_NONE;
                //}, 1500 - ControlMotor::currentSpeed() * 10);
                }, movement*100);
            }

            break;
        }

        case (SERVO_TURN_RIGHT): {
            Log::logMessage(PI, LOG_DEBUG, "Servo request turn right");
            if (_servoCntl == SERVO_NONE ||
                _servoCntl == SERVO_ADJUST_LEFT ||
                _servoCntl == SERVO_ADJUST_RIGHT) {
                _servoCntl = SERVO_TURN_RIGHT;
                request(SERVO_RIGHT, 100);
                if (movement=0)
                    movement = 30;

                Log::logMessage(PI, LOG_DEBUG, "Servo turn right timer set");
                _timer.setTimeout([&]() {
                    Log::logMessage(PI, LOG_DEBUG, "Servo turn right timer timed out");
                    request(SERVO_CENTER);

                    _timer.stop();
                    _servoCntl = SERVO_NONE;
               // }, 1500 - ControlMotor::currentSpeed() * 10);
                }, movement*100);

            }
            break;
        }

        case (SERVO_3_LEFT):{
            Log::logMessage(PI, LOG_DEBUG, "Servo request turn 3 left");
            if (_servoCntl == SERVO_NONE ||
                _servoCntl == SERVO_ADJUST_RIGHT ||
                _servoCntl == SERVO_ADJUST_LEFT) {
                _servoCntl = SERVO_TURN_LEFT;
                request(SERVO_LEFT, 100);
                Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 left timer 1 set");
                _timer.setTimeout([&]() {
                    Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 left timer 1 timed out");
                    request(SERVO_RIGHT, 100);
                    ControlMotor::request(MOTOR_MOVE_BACK);
                    _timer.stop();
                    Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 left timer 2 set");
                    _timer.setTimeout([&]() {
                        Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 left timer 1 timed out");
                        request(SERVO_CENTER);
                        ControlMotor::request(MOTOR_MOVE_FORWARD);
                        _servoCntl = SERVO_NONE;
                        _timer.stop();
                    }, 2000); //additional time for change of direction
                }, 1550);
            }
            break;

        }

        case (SERVO_3_RIGHT):{
            Log::logMessage(PI, LOG_DEBUG, "Servo request turn 3 right");
            if (_servoCntl == SERVO_NONE ||
                _servoCntl == SERVO_ADJUST_LEFT ||
                _servoCntl == SERVO_ADJUST_RIGHT) {
                _servoCntl = SERVO_TURN_RIGHT;
                request(SERVO_RIGHT, 100);
                Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 right timer 1 set");
                _timer.setTimeout([&]() {
                    Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 right timer 1 timed out");
                    request(SERVO_LEFT, 100);
                    ControlMotor::request(MOTOR_MOVE_BACK);
                    _timer.stop();
                    Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 right timer 2 set");
                    _timer.setTimeout([&]() {
                        Log::logMessage(PI, LOG_DEBUG, "Servo turn 3 right timer 2 timed out");
                        request(SERVO_CENTER);
                        ControlMotor::request(MOTOR_MOVE_FORWARD);
                        _servoCntl = SERVO_NONE;
                        _timer.stop();
                    }, 2000); //additional time for change of direction
                }, 15500);
            }
            break;
        }

        case (SERVO_ADJUST_LEFT):{
            Log::logMessage(PI, LOG_DEBUG, "Servo request adjust left");
            if (_servoCntl == SERVO_NONE) {
                _servoCntl = SERVO_ADJUST_LEFT;
                request(SERVO_LEFT, 75);
                Log::logMessage(PI, LOG_DEBUG, "Servo adjust left timer set");
                _timer.setTimeout([&]() {
                    Log::logMessage(PI, LOG_DEBUG, "Servo adjust left timer timed out");
                    if (processingServoRqst()==SERVO_ADJUST_LEFT) {
                        request(SERVO_CENTER);
                        _servoCntl == SERVO_NONE;
                    }
                    _timer.stop();
                }, 2000);
            }
            break;

        }
        case (SERVO_ADJUST_RIGHT):{
            Log::logMessage(PI, LOG_DEBUG, "Servo request adjust right");
            if (_servoCntl == SERVO_NONE) {
                _servoCntl == SERVO_ADJUST_RIGHT;
                request(SERVO_RIGHT, 75);
                Log::logMessage(PI, LOG_DEBUG, "Servo adjust right timer set");
                _timer.setTimeout([&]() {
                    Log::logMessage(PI, LOG_DEBUG, "Servo adjust right timer timed out");
                    if (processingServoRqst() == SERVO_ADJUST_RIGHT) {
                        request(SERVO_CENTER);
                        _servoCntl == SERVO_NONE;
                    }

                    _timer.stop();
                    //  }, 310 - ControlMotor::currentSpeed()*10);
                }, 2000);
            }
            break;
        }
        default:{
            break;
        }
    }
}

ServoCntl ControlServo::processingServoRqst() {
    return _servoCntl;
}

void ControlServo::setArduino(SerialIO *arduinoIO) {
    _arduinoIO = *arduinoIO;
}

