//
// Created by james on 10/03/19.
//

#ifndef ROBLUCKSPI_LOG_H
#define ROBLUCKSPI_LOG_H

#include "../SerialIO/SerialIO.h"
#include "json.hpp"

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4
};

enum MessageSource {
    ARDUINO,
    PI
};

class Log {
private:
    static SerialIO _nodeRedIO;
    static LogLevel _minReportLevel;
    static bool _outputToScreen;

public:
    static void logMessage(MessageSource messageSource, LogLevel logLevel, std::string messageToSend);
    static void logMessage(MessageSource messageSource, std::string logLevel, std::string messageToSend);
    static void setNodeRed(SerialIO *nodeRedIO);
    static void setLogLevel(LogLevel logLevel);
};


#endif //ROBLUCKSPI_LOG_H
