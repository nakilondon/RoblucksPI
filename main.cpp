#include "Message.h"
#include <unistd.h>

#include "SerialIO/SerialIO.h"
#include "parameters.h"
#include "Joystick/RoblucksJoystick.h"
#include "ManualControl/ManualControl.h"
#include "AutonomousControl/AutonomousControl.h"
#include "ControlServoMotor/ControlServoMotor.h"

bool isConnected = false;
SerialIO ardunioIO("/dev/arduino-roblucks", 115200);
SerialIO nodeRedIO("/dev/tnt1", 115200);
AutonomousControl autonomousControl;
Mode currentMode = DEFAULT_MODE;
ManualControl manualControl;
ControlServoMotor controlServoMotor;

Message getMessageFromArdunio();
void getMessageFromNodeRed();

int main() {
    RoblucksJoystick joystick;
    setbuf(stdout, 0);

    Message message = HELLO;

    if (!ardunioIO.Open()) {
        fprintf(stderr, "Unable to open serial port\n");
        exit(1);
    }

    if (!nodeRedIO.Open()) {
        fprintf(stderr, "Unable to open Node-Red port\n");
        exit(1);
    }

   controlServoMotor.setArduino(&ardunioIO);

    while (!isConnected){
        ardunioIO.Send(&message,1);
        if(ardunioIO.WaitForBytes(1, 1000))
            if(getMessageFromArdunio()==HELLO)
                message=ALREADY_CONNECTED;
    }

    while (true) {
        // Restrict rate
        usleep(10);

        int movement = 0;
        auto joystickType = joystick.checkJoystick(movement);

        if (joystickType != joyNone) {
            if (currentMode == MANUAL)
                manualControl.joystickCommand(joystickType, movement, controlServoMotor);
            else
                autonomousControl.joystickCommand(joystickType, controlServoMotor);
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
            Direction direction = static_cast<Direction >(ardunioIO.Read());
            uint8_t measure = ardunioIO.Read();

            autonomousControl.processDistance(direction, measure, controlServoMotor, &currentMode);

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

    std::string msgStr;
    bool timeout = false;
    bool msgEnd = false;

    while (!msgEnd && !timeout) {
        if (nodeRedIO.WaitForBytes(1, 10000)) {
            msgStr += nodeRedIO.Read();
            if (msgStr.length()>2)
                if (msgStr.substr(msgStr.length()-1,1)=="\n")
                    msgEnd = true;
        } else
            timeout = true;
    }

    if (timeout)
        fprintf(stderr, "String from Node-Red timedout, partial msg: %s\n", msgStr.c_str());
    else {
        fprintf(stdout, "Message from Node-Red: %s\n", msgStr.c_str());
        if(msgStr.find("setMode")){
            if(msgStr.find("Autonomus")) {
                currentMode = AUTONOMOUS;
                fprintf(stdout, "Set mode to autonomus");
            } else if(msgStr.find("Manual")) {
                currentMode = MANUAL;
                fprintf(stdout, "Set mode to manual");
            }
        }
    }
}