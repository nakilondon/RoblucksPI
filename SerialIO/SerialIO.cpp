//
// Created by james on 17/02/19.
//

#include "SerialIO.h"
extern "C" {
#include <asm/termbits.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
}

#include <chrono>

using Clock = std::chrono::system_clock;

SerialIO::SerialIO(std::string deviceName, int baud) {
    _handle = -1;
    _deviceName = deviceName;
    _baud = baud;
}

bool SerialIO::Open()
{
    struct termios tio;
    struct termios2 tio2;

    _handle  = open(_deviceName.c_str(),O_RDWR | O_NOCTTY | O_NDELAY /* | O_NONBLOCK */);

    if(_handle <0)
        return false;
    tio.c_cflag =  CS8 | CLOCAL | CREAD;
    tio.c_oflag = 0;
    tio.c_lflag = 0;       //ICANON;
    tio.c_cc[VMIN]=0;
  //  tio.c_cc[VTIME]=1;     // time out every .1 sec
    ioctl(_handle,TCSETS,&tio);

    ioctl(_handle,TCGETS2,&tio2);
    tio2.c_cflag &= ~CBAUD;
    tio2.c_cflag |= BOTHER;
    tio2.c_ispeed = _baud;
    tio2.c_ospeed = _baud;
    ioctl(_handle,TCSETS2,&tio2);

//   flush buffer
    ioctl(_handle,TCFLSH,TCIOFLUSH);

//    sleep(2);
    return true;
}

bool SerialIO::Send(const void *msg, size_t len) {
    int wLen = write (_handle, msg, len);
    return wLen == len;
}

bool SerialIO::Send(std::string msg) {
    return Send(msg.c_str(), msg.length());
}
/*
int SerialIO::Read() {
    uint8_t x;
    if (read(_handle, &x,1) != 1)
        return -1;

    return ((int)x) & 0xFF;
}*/
uint8_t SerialIO::Read() {
    uint8_t x;
    read(_handle, &x, 1);
    return x;
}

int SerialIO::ReadInt() {
    int x;

    read(_handle, &x, sizeof(x) );
    return x;
}


short SerialIO::ReadShort() {
    short x;

    read(_handle, &x, sizeof(x) );
    return x;
}

int SerialIO::BytesQued() {
    int bytelen;
    ioctl(_handle, FIONREAD, &bytelen);
    return bytelen;
}

bool SerialIO::WaitForBytes(int numBytes, unsigned long timeout) {
    using namespace std::chrono;
    milliseconds strTime = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );
    milliseconds nowTime = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );

    while (BytesQued() < numBytes && nowTime < strTime +(milliseconds)timeout ){
        nowTime = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
    }

    if (BytesQued()>0)
        return true;

    return false;

}

SerialIO::~SerialIO() {
    if(_handle>=0)
        close(_handle);
}

void SerialIO::FlushBuffer() {
    ioctl(_handle,TCFLSH,TCIOFLUSH);
}

bool SerialIO::handlerOpen() {
    return (_handle>=0);
}