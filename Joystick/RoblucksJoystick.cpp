//
// Created by james on 04/03/19.
//

#include "RoblucksJoystick.h"

JoystickType RoblucksJoystick::checkJoystick(int &movement) {
    JoystickEvent event;
    movement = 0;

    if (_joystick.sample(&event)) {
        if (event.isButton()) {
            switch (event.number) {
                case 0: {
                    if (event.value == 1) {
                        return joyStop;
                    }
                    break;
                }
                case 1: {
                    if (event.value == 1) {
                        return joy3Right;
                    }
                    break;
                }
                case 2: {
                    if (event.value == 1) {
                        return joy3Left;
                    }
                    break;
                }
                case 3: {
                    if (event.value == 1) {
                        return joyStart;
                    }
                    break;
                }
                case 4: {
                    if (event.value == 1)
                        return joyIncreaseSpeed;

                    break;
                }
                case 5: {
                    if (event.value == 1)
                        return joyDecreaseSpeed;

                    break;
                }
                default: {
                    break;
                }
            }
        } else if (event.isAxis()) {
            switch (event.number) {
                case 1: {
                    if (event.value == 0)
                        return joyStop;

                    if (event.value < 0)
                        return joyMoveForward;

                    return joyMoveBack;
                }

                case 2: {
                    if (event.value == 0)
                        return joyCenter;

                    JoystickType type;
                    if (event.value > 0) {
                        type = joyMoveRight;
                    } else {
                        type = joyMoveLeft;
                    }

                    movement =  static_cast<int>(_servoScale * abs(event.value) + 0.5);

                    return type;

                }
                default: {
                    break;
                }
            }
        }
    }

    return joyNone;
}

void RoblucksJoystick::resetJoystick() {
    (&_joystick)->~Joystick();
    new (&_joystick) Joystick();
}

bool RoblucksJoystick::isFound() {
    return _joystick.isFound();
}