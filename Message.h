//
// Created by james on 16/02/19.
//

enum Message {
    HELLO = 1,
    SERVO = 2,
    MOTOR = 3,
    ALREADY_CONNECTED = 4,
    ERROR = 5,
    RECEIVED = 6,
    STRING = 7
};

typedef enum Message Message;

enum MotorCmd {
    FORWARD = 1,
    REVERSE = 2,
    STOP = 3,
    ARM = 4
};

enum ServoCmd{
    CENTER = 1,
    LEFT = 2,
    RIGHT = 3
};

