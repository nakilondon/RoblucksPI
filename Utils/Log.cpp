//
// Created by james on 10/03/19.
//

#include "Log.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

SerialIO Log::_nodeRedIO = SerialIO("",11520);
LogLevel Log::_minReportLevel = LOG_DEBUG;
bool Log::_outputToScreen = true;

using json = nlohmann::json;
void Log::logMessage(MessageSource messageSource, std::string logLevel, std::string messageToSend) {
    LogLevel level;

    if (logLevel=="0")
        level = LOG_DEBUG;
    else if (logLevel=="1")
        level = LOG_INFO;
    else if (logLevel=="2")
        level = LOG_WARNING;
    else if (logLevel=="3")
        level = LOG_ERROR;
    else if (logLevel == "4")
        level = LOG_CRITICAL;
    else {
        logMessage(PI, LOG_ERROR, "Unknown log level: " + logLevel + " message to send: " + messageToSend);
        return;
    }

    logMessage(messageSource, level, messageToSend);

}

void Log::logMessage(MessageSource messageSource, LogLevel logLevel, std::string messageToSend) {


    if (logLevel < _minReportLevel)
        return;

    json jsonOutput;

   std::string msgSource;

    if(messageSource == ARDUINO)
        msgSource = "Arduino";
    else
        msgSource = "PI";

    jsonOutput["messageSource"] = msgSource;
    jsonOutput["timeStamp"] = getTimeStamp();
    jsonOutput["logLevel"] = logLevel;
    jsonOutput["msgDetail"] = messageToSend;

 //   _nodeRedIO.Send(jsonOutput.dump()+"\n");

    if (_outputToScreen){
        fprintf(stdout, "%s\n", jsonOutput.dump().c_str());
    }
}

std::string Log::getTimeStamp() {
    using namespace std::chrono;
    auto now = std::chrono::_V2::system_clock::now();
    time_t now_c = std::chrono::_V2::system_clock::to_time_t(now);
    const auto nowMs = duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
    std::stringstream timeNow;
    timeNow << std::put_time(localtime(&now_c), "%Y-%m-%d %T")
            << '.' << std::setfill('0') << std::setw(3) << nowMs.count();
    return timeNow.str();
}

void Log::setNodeRed(SerialIO *nodeRedIO) {
    _nodeRedIO = *nodeRedIO;
}

void Log::setLogLevel(LogLevel logLevel) {
    _minReportLevel = logLevel;
}

