#include "Message.h"
#include <unistd.h>

#include "SerialIO/SerialIO.h"
#include "parameters.h"
#include "Joystick/RoblucksJoystick.h"
#include "ManualControl/ManualControl.h"
#include "AutonomousControl/AutonomousControl.h"
#include "Controls/ControlServo.h"
#include "Utils/json.hpp"
#include "Utils/Log.h"

using json = nlohmann::json;

bool isConnected = false;
SerialIO ardunioIO("/dev/arduino-roblucks", 115200);
SerialIO nodeRedIO("/dev/tnt1", 115200);
Mode currentMode = DEFAULT_MODE;
Message getMessageFromArdunio();

void getMessageFromNodeRed();

int main() {
    RoblucksJoystick joystick;
    setbuf(stdout, 0);

    Message message = HELLO;

    if (!ardunioIO.Open()) {
        Log::logMessage(PI, LOG_CRITICAL, "Unable to open Arduino port");
        exit(1);
    }

    if (!nodeRedIO.Open()) {
        Log::logMessage(PI, LOG_CRITICAL, "Unable to open Node-Red port");
        exit(1);
    }

    ControlServo::setArduino(&ardunioIO);
    ControlMotor::setArduino(&ardunioIO);
    Log::setNodeRed(&nodeRedIO);

    while (!isConnected){
        ardunioIO.Send(&message,1);
        if(ardunioIO.WaitForBytes(1, 1000))
            if(getMessageFromArdunio()==HELLO)
                message=ALREADY_CONNECTED;
    }

    if (currentMode != AUTONOMOUS) {
        char msgToSend[] = {OPERATION, TURN_DISTANCES_OFF};
        ardunioIO.Send(msgToSend, sizeof(msgToSend));
    }


    while (true) {
        // Restrict rate
        usleep(10);

        int movement = 0;
        auto joystickType = joystick.checkJoystick(movement);

        if (joystickType != joyNone) {
            if (currentMode == MANUAL)
                ManualControl::joystickCommand(joystickType, movement);
            else
                AutonomousControl::joystickCommand(joystickType);
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
            Log::logMessage(PI, LOG_INFO, "Connected");
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
                    Log::logMessage(PI, LOG_ERROR, "String from timedout, partial msg: " + msgStr);
                else {
                    Log::logMessage(ARDUINO, logLevel, msgStr.substr(0,msgStr.length()-2));
                }
            }
            return LOG;
            break;
        }
        case DISTANCE:{
            DistanceSensor direction = static_cast<DistanceSensor >(ardunioIO.Read());
            uint8_t measure = ardunioIO.Read();

            if (currentMode==AUTONOMOUS)
                AutonomousControl::processDistance(direction, measure);
            else {
                char msgToSend[] = {OPERATION, TURN_DISTANCES_OFF};
                ardunioIO.Send(msgToSend, sizeof(msgToSend));
            }

            return DISTANCE;
            break;
        }
        case SERVO:{
            Log::logMessage(PI, LOG_DEBUG, "SERVO message from ardunio");
            return SERVO;
            break;
        }
        case MOTOR:{
            Log::logMessage(PI, LOG_DEBUG, "MOTOR message from ardunio");
            return MOTOR;
            break;
        }
        case ALREADY_CONNECTED:{
            isConnected = true;
            Log::logMessage(PI, LOG_DEBUG, "ALREADY_CONNECTED message from ardunio");
            return ALREADY_CONNECTED;
            break;
        }

        default: {
            Log::logMessage(PI, LOG_ERROR, "Unknown message from ardunio");
            ardunioIO.FlushBuffer();
            break;
        }

    }
    return ERROR;
}

void getMessageFromNodeRed() {
    if (nodeRedIO.BytesQued() <= 0)
        return;

    std::string msgStr;
    msgStr.clear();
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
        Log::logMessage(PI, LOG_ERROR, "String from Node-Red timedout, partial msg: " + msgStr);
    else {
        Log::logMessage(PI, LOG_DEBUG, "Message from Node-Red: " + msgStr);

        try {
            json nodeRedJson = json::parse(msgStr);
            std::string operation;
            try {
                operation = nodeRedJson["operation"].get<std::string>();
            } catch (const std::exception &e) {
                Log::logMessage(PI, LOG_ERROR, "Unable to find operation within node red message exception: " + std::string(e.what()));
            }
            if (operation == "setMode") {
                std::string newMode;
                try {
                    newMode = nodeRedJson["mode"].get<std::string>();
                } catch (const std::exception &e) {
                    Log::logMessage(PI, LOG_DEBUG, "Unable to find operation within node red message excpetion: " + std::string(e.what()));
                }
                if (newMode=="Auto"){
                    currentMode = AUTONOMOUS;
                    char msgToSend[] = {OPERATION, TURN_DISTANCES_ON};
                    ardunioIO.Send(msgToSend, sizeof(msgToSend));
                    Log::logMessage(PI, LOG_DEBUG, "Autonomus opertion set");
                } else {
                    if (newMode=="Manual"){
                        currentMode = MANUAL;
                        char msgToSend[] = {OPERATION, TURN_DISTANCES_OFF};
                        ardunioIO.Send(msgToSend, sizeof(msgToSend));
                        Log::logMessage(PI, LOG_DEBUG, "Manual operation");
                    }
                    else {
                        Log::logMessage(PI, LOG_ERROR, "Invalid mode: %s " + newMode);
                    }
                }
            } else {
                Log::logMessage(PI, LOG_DEBUG, "Invalid operation:  " + operation);
            }


        } catch (const std::exception& e) {
            Log::logMessage(PI, LOG_DEBUG, "Unable to parse json from node red, exception: " + std::string(e.what()));

        }
    }
}