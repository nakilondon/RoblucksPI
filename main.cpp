#include "Message.h"
#include <unistd.h>

#include "SerialIO/SerialIO.h"
//#include "Joystick/Joystick.h"
#include "parameters.h"
#include <list>
#include "Joystick/RoblucksJoystick.h"
#include "ManualControl/ManualControl.h"
#include "AutonomousControl/AutonomousControl.h"
/*
enum Adjustment {
    NONE,
    ADJUST_LEFT,
    ADJUST_RIGHT,
    TURNING_LEFT,
    TURNING_RIGHT
};*/

bool isConnected = false;
Adjustment adjustingPosition = NONE;
SerialIO ardunioIO("/dev/arduino-roblucks", 115200);
SerialIO nodeRedIO("/dev/tnt1", 115200);
Mode currentMode = DEFAULT_MODE;

struct distances {
    time_t recived;
    short distance;
};

std::list<uint8_t > leftDistances;
std::list<uint8_t > rightDistances;

MotorCmd currentMotorCmd;
uint8_t currentSpeed;

Message getMessageFromArdunio();
void getMessageFromNodeRed();

int main() {
    Joystick joystick("/dev/input/js0");
    setbuf(stdout, 0);

    // Ensure that it was found and that we can use it
    if (!joystick.isFound())
    {
        fprintf(stderr, "Joystick open failed.\n");
        exit(1);
    }

    Message message = HELLO;

    if (!ardunioIO.Open()) {
        fprintf(stderr, "Unable to open serial port\n");
        exit(1);
    }

    if (!nodeRedIO.Open()) {
        fprintf(stderr, "Unable to open Node-Red port\n");
        exit(1);
    }

    while (!isConnected){
        ardunioIO.Send(&message,1);
        if(ardunioIO.WaitForBytes(1, 1000))
            if(getMessageFromArdunio()==HELLO)
                message=ALREADY_CONNECTED;
    }

    while (true) {
        // Restrict rate
        usleep(100);

        int movement = 0;
        auto joystickType = RoblucksJoystick::checkJoystick(movement, &joystick);

        if (joystickType != joyNone) {
            if (currentMode = MANUAL)
                ManualControl::joystickCommand(joystickType, movement, &ardunioIO, currentSpeed, currentMotorCmd);
            else
                AutonomousControl::joystickCommand(joystickType, movement, &ardunioIO, currentSpeed, currentMotorCmd);
        }

        if (ardunioIO.BytesQued()>0)
            getMessageFromArdunio();

        if (nodeRedIO.BytesQued()>0)
            getMessageFromNodeRed();
    }
}

Message getMessageFromArdunio() {
    if (ardunioIO.BytesQued() <= 0)
        return ERROR;

    Message msgRecv = (Message)ardunioIO.Read();

    switch (msgRecv) {
        case HELLO:{
            fprintf(stdout, "Connected\n");
            return HELLO;
            break;
        }

        case LOG: {
            if (ardunioIO.WaitForBytes(1, 100)) {
                std::string logLevel;
                logLevel = ardunioIO.Read();
                std::string msgStr;
                bool timeout = false;
                bool msgEnd = false;

                while (!msgEnd && !timeout) {
                    if (ardunioIO.WaitForBytes(1, 10000)) {
                        msgStr += ardunioIO.Read();
                        if (msgStr.length()>2)
                            if (msgStr.substr(msgStr.length()-2,2)=="\r\n")
                                msgEnd = true;
                    } else
                        timeout = true;
                }
                if (timeout)
                    fprintf(stderr, "String from timedout, partial msg: %s\n", msgStr.c_str());
                else {
                    std::string msgOut = "{ \"type\":\"log\", ";
                    msgOut += "\"source\":\"Arduino\", ";
                    msgOut += "\"LogLevel\":";
                    msgOut += logLevel +",";
                    msgOut += "\"msgDetail\":\"";
                    msgOut += msgStr.substr(0,msgStr.length()-2);
                    msgOut += "\" }\n";

                    nodeRedIO.Send(msgOut);
                }
            }
            return LOG;
            break;
        }
        case DISTANCE:{
            AutonomousControl::processDistance(&ardunioIO, currentSpeed, currentMotorCmd,
                    adjustingPosition, leftDistances, rightDistances);
            /*Direction direction = static_cast<Direction >(ardunioIO.Read());
            uint8_t measure = ardunioIO.Read();

            if (direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT){
                uint8_t leftDistance;
                uint8_t rightDistance;
                if(direction==DIRECTION_LEFT) {
                    leftDistances.push_front(measure);
                    leftDistance = measure;
                    if (rightDistances.empty())
                        return DISTANCE;
                    rightDistance = rightDistances.front();
                    rightDistances.pop_back();

                } else {
                    rightDistances.push_front(measure);
                    rightDistance = measure;
                    if (leftDistances.empty())
                        return DISTANCE;
                    leftDistance = leftDistances.front();
                    leftDistances.pop_back();
                }

                auto totalLength = rightDistance + leftDistance;
                auto difference = abs(rightDistance - leftDistance);
                auto middel = totalLength /2;

                switch (adjustingPosition) {
                    case NONE: {
                        if (difference >= middel * 0.2) {
                            char messageToSend[3] = {SERVO};
                            if ((currentMotorCmd == FORWARD && rightDistance < leftDistance) ||
                                currentMotorCmd == REVERSE && rightDistance > leftDistance) {
                                messageToSend[1] = LEFT;
                                adjustingPosition = ADJUST_LEFT;
                            //    double servoDistance = abs(middel - leftDistance);
                            //    servoDistance = servoDistance / middel;
                            //    servoDistance = servoDistance * 100;
                            //    messageToSend[2] = static_cast<uint8_t>(servoDistance);
                            } else {
                                messageToSend[1] = RIGHT;
                                adjustingPosition = ADJUST_RIGHT;
                              //  double servoDistance = abs(middel - rightDistance);
                              //  servoDistance = servoDistance / middel;
                              //  servoDistance = servoDistance * 100;
                              //  messageToSend[2] = static_cast<uint8_t>(servoDistance);
                            }
                            messageToSend[2] = 50;
                            ardunioIO.Send(messageToSend, sizeof(messageToSend));
                        }
                        break;
                    }

                    case ADJUST_LEFT:
                    case ADJUST_RIGHT: {
                        if (difference <= middel *.1) {
                            char messageToSend[] = {SERVO, CENTER};
                            ardunioIO.Send(messageToSend, sizeof(messageToSend));
                            adjustingPosition = NONE;
                        }
                        break;
                    }
                }
            }

            if (DIRECTION_FRONT && measure <= 5 && measure > 0){
                char messageToSend[] = {MOTOR,STOP};
                ardunioIO.Send(messageToSend, sizeof(messageToSend));

            }*/
            return DISTANCE;
            break;
        }
        case SERVO:{
            fprintf(stdout, "SERVO message from ardunio\n");
            return SERVO;
            break;
        }
        case MOTOR:{
            fprintf(stdout, "MOTOR message from ardunio\n");
            return MOTOR;
            break;
        }
        case ALREADY_CONNECTED:{
            isConnected = true;
            fprintf(stdout, "ALREADY_CONNECTED message from ardunio\n");
            return ALREADY_CONNECTED;
            break;
        }

        default: {
            fprintf(stdout, "Unknown message from ardunio\n");
            break;
        }

    }
    return ERROR;
}

void getMessageFromNodeRed() {
    if (nodeRedIO.BytesQued() <= 0)
        return;

    char msgStr[100] = {0};
    int i = 0;
    bool timeout = false;

    while (i < sizeof(msgStr) && msgStr[i] != '\n' && msgStr[i] != '\r' && !timeout) {
        if (nodeRedIO.WaitForBytes(1, 10000)) {
            msgStr[i] = nodeRedIO.Read();
            if (msgStr[i] == '\n' || msgStr[i] == '\r') {
                msgStr[i + 1] = 0;
            } else {
                i++;
            }
        } else
            timeout = true;
    }

    if (timeout)
        fprintf(stderr, "String from Node-Red timedout, partial msg: %s\n", msgStr);
    else {

        fprintf(stdout, "Message from Node-Red: %s\n", msgStr);

    }
}