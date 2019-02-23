//
// Created by james on 17/02/19.
//

#ifndef ROBLUCKSPI_SERIALIO_H
#define ROBLUCKSPI_SERIALIO_H

#include <string>

class SerialIO {
private:

    std::string _deviceName;
    int _baud;
    int _handle;


public:

    SerialIO(std::string deviceName, int baud);
    ~SerialIO();

    bool Open();

    bool Send(const void *msg, size_t len);
    bool Send(std::string);
    //int Read();
    uint8_t Read();
    int BytesQued();
    bool WaitForBytes(int numBytes, unsigned long timeout);
};


#endif //ROBLUCKSPI_SERIALIO_H
