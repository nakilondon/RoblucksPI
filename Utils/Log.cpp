//
// Created by james on 10/03/19.
//

#include "Log.h"

SerialIO Log::_nodeRedIO = SerialIO("",11520);
LogLevel Log::_minReportLevel = LOG_INFO;
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
    jsonOutput["logLevel"] = logLevel;
    jsonOutput["msgDetail"] = messageToSend;

    _nodeRedIO.Send("HI\n");

    _nodeRedIO.Send(jsonOutput.dump()+"\n");

    if (_outputToScreen){
        fprintf(stdout, "%s\n", jsonOutput.dump().c_str());
    }
}

void Log::setNodeRed(SerialIO *nodeRedIO) {
    _nodeRedIO = *nodeRedIO;
}

void Log::setLogLevel(LogLevel logLevel) {
    _minReportLevel = logLevel;
}

