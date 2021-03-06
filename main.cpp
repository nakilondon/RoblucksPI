#include "Message.h"
#include <unistd.h>

#include "SerialIO/SerialIO.h"
#include "parameters.h"
#include "Joystick/RoblucksJoystick.h"
#include "Controls/ManualControl.h"
#include "Controls/AutonomousControl.h"
#include "Controls/ControlServo.h"
#include "Utils/json.hpp"
#include "Utils/Log.h"
#include <chrono>
#include <fstream>

using json = nlohmann::json;

bool isConnected = false;
SerialIO ardunioIO("/dev/arduino-roblucks", 115200);
SerialIO nodeRedIO("/dev/tnt1", 115200);
RoblucksJoystick joystick;

Mode currentMode = DEFAULT_MODE;

Message getMessageFromArdunio();
void startUp();

void getMessageFromNodeRed();

void checkJoystick();

int main() {

    setbuf(stdout, 0);

    startUp();

    while (true) {
        // Restrict rate
        usleep(1000);

        checkJoystick();

        if (ardunioIO.BytesQued()>0)
            getMessageFromArdunio();

//        if (nodeRedIO.BytesQued()>0)
//            getMessageFromNodeRed();
    }
}

void checkJoystick() {
    int movement = 0;
    auto joystickType = joystick.checkJoystick(movement);

    if (joystickType != joyNone) {
            if (currentMode == MANUAL)
                ManualControl::joystickCommand(joystickType, movement);
            else {
                if (joystickType == joyStart ){
                    Log::logMessage(PI, LOG_DEBUG, "Auto Start requested");
                    char msgToSend[] = {OPERATION, TURN_DISTANCES_ON};
                    ardunioIO.Send(msgToSend, sizeof(msgToSend));
                    usleep(100000);
                } else if (joystickType == joyStop) {
                    char msgToSend[] = {OPERATION, TURN_DISTANCES_OFF};
                    ardunioIO.Send(msgToSend, sizeof(msgToSend));
                }
                AutonomousControl::joystickCommand(joystickType);

            }
        }
}

void startUp() {

    using namespace std::chrono;

    char message[] = {HELLO};

    while (!ardunioIO.handlerOpen() ||
       // !nodeRedIO.handlerOpen() ||
            !joystick.isFound() ||
            !isConnected) {

  //      if (!nodeRedIO.handlerOpen()) {
  //          if (!nodeRedIO.Open()) {
  //              Log::logMessage(PI, LOG_CRITICAL, "Unable to open Node-Red port");
  //          }
  //      }

        if (!ardunioIO.handlerOpen()) {
            if (!ardunioIO.Open()) {
                Log::logMessage(PI, LOG_CRITICAL, "Unable to open Arduino port");
            } else {
                ardunioIO.FlushBuffer();

                for (int i=0; i < 30; i++) { // ignore first messages
                    ardunioIO.Read();
                }
            }
        }

        if (!joystick.isFound()) {
            Log::logMessage(PI, LOG_CRITICAL, "Unable to find joystick");
            joystick.resetJoystick();
        }

        milliseconds strTime = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );

        milliseconds nowTime = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );

        while (ardunioIO.handlerOpen() & !isConnected && nowTime < strTime +(milliseconds)10000 )
            {
            if (message[0]==HELLO)
                Log::logMessage(PI, LOG_INFO, "Sending Hello");
            else
                Log::logMessage(PI, LOG_INFO, "Sending Already Connected");
            ardunioIO.Send(message, sizeof(message));
            usleep(1000);

            if (ardunioIO.WaitForBytes(1, 1000)) {
                if (getMessageFromArdunio() == HELLO) {
                    message[0] = ALREADY_CONNECTED;
                }
            }

            nowTime = duration_cast< milliseconds >(
                      system_clock::now().time_since_epoch());
        }
        if (!ardunioIO.handlerOpen() ||
    //     !nodeRedIO.handlerOpen() ||
         !joystick.isFound() ||
         !isConnected)
            sleep(10);
    }

    ControlServo::setArduino(&ardunioIO);
    ControlMotor::setArduino(&ardunioIO);
   // Log::setNodeRed(&nodeRedIO);


    std::ifstream configFile("/home/pi/roblucks/config.json");

    json configJson;

    configFile >> configJson;

    auto currentConfig = configJson["currentConfig"].get<std::string>();

    if (currentConfig=="manual"){
        currentMode = MANUAL;
        ManualControl::setup(configJson["manual"]);
    } else if (currentConfig=="auto") {
        currentMode = AUTONOMOUS;
        AutonomousControl::setup(configJson["auto"]);
    } else {
        Log::logMessage(PI, LOG_CRITICAL, "Unknown mode of " + currentConfig + "in config.json");
    }

    if (currentMode == AUTONOMOUS) {
        Log::logMessage(PI, LOG_DEBUG, "Turning on distances");
    } else {
        Log::logMessage(PI, LOG_DEBUG, "Turning off distances");
    }

    auto logConfig = configJson["logging"];

    auto minLogLevels = logConfig["minLevel"];

    auto ardunioLevel = minLogLevels["arduino"].get<uint8_t >();

    Log::logMessage(PI, LOG_DEBUG, "Setting arduino log level");
    char msgToSend[] = {OPERATION, SET_LOG_LEVEL, ardunioLevel};
    ardunioIO.Send(msgToSend, sizeof(msgToSend));

    auto piLevel = minLogLevels["pi"].get<uint8_t >();

    Log::setLogLevel(static_cast<LogLevel>(piLevel));

}


Message getMessageFromArdunio() {
    if (ardunioIO.BytesQued() <= 0)
        return ERROR;

    Message msgRecv = (Message)ardunioIO.Read();

    switch (msgRecv) {
        case HELLO:{
            Log::logMessage(PI, LOG_INFO, "Connected");
            usleep(100);
            uint8_t alreadyConnected[] = {ALREADY_CONNECTED};
            ardunioIO.Send(alreadyConnected,1);
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
                            if (msgStr.substr(msgStr.length()-2,2)=="\r\n" ||
                                msgStr.substr(msgStr.length()-2,2)=="\r\r")
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
            if(ardunioIO.WaitForBytes(1,1000)) {
                DistanceSensor direction = static_cast<DistanceSensor >(ardunioIO.Read());
                if(ardunioIO.WaitForBytes(2,1000)) {
                    short measure = ardunioIO.ReadShort();

                 //   Log::logMessage(PI, LOG_DEBUG, "Distance, sensor: " + std::to_string(direction) + " measure: " +
                 //                                  std::to_string(measure));

                    if (measure > 100) {
                        Log::logMessage(PI, LOG_ERROR, "Invalid measure recevied, flushing buffer");
                        //  ardunioIO.FlushBuffer();
                    }

                    if (currentMode == AUTONOMOUS)
                        AutonomousControl::processDistance(direction, measure);
                    else {
                        char msgToSend[] = {OPERATION, TURN_DISTANCES_OFF};
                        ardunioIO.Send(msgToSend, sizeof(msgToSend));
                    }
                }
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
            usleep(1000);
            return ALREADY_CONNECTED;
            break;
        }

        default: {
            Log::logMessage(PI, LOG_ERROR, "Unknown message from ardunio");
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
                if (msgStr.substr(msgStr.length()-2,2)=="\r\r")
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
                if (newMode=="auto"){
                    currentMode = AUTONOMOUS;
                    auto autoConfig = nodeRedJson["auto"];
                    char msgToSend[] = {OPERATION, TURN_DISTANCES_ON};
                    ardunioIO.Send(msgToSend, sizeof(msgToSend));
                    AutonomousControl::setup(autoConfig);
                    Log::logMessage(PI, LOG_DEBUG, "Autonomus opertion set");
                } else {
                    if (newMode=="manual"){
                        currentMode = MANUAL;
                        auto manualConfig = nodeRedJson["manual"];
                        char msgToSend[] = {OPERATION, TURN_DISTANCES_OFF};
                        ardunioIO.Send(msgToSend, sizeof(msgToSend));
                        ManualControl::setup(manualConfig);
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