#include "Message.h"
#include "SerialIO/SerialIO.h"

bool isConnected = false;
SerialIO serialIO("/dev/ttyACM0", 115200);

Message getMessage();

int main() {
    int fd;

    Message message = HELLO;

    if (!serialIO.Open()) {
        fprintf(stderr, "Unable to open serial port");
    }

    while (!isConnected){
        serialIO.Send(&message,1);
        if(serialIO.WaitForBytes(1, 100))
            if(getMessage()==HELLO)
                message=ALREADY_CONNECTED;
    }

    char msgToSend[] = {MOTOR, FORWARD, 13};
    serialIO.Send(msgToSend, sizeof(msgToSend));

    while (getMessage() != STRING){}

    char servoToSend[] = {SERVO, LEFT, 99};
    serialIO.Send(servoToSend, sizeof(servoToSend));

    while (getMessage() != STRING){}
}

Message getMessage() {
    if (!serialIO.WaitForBytes(1, 100))
        return ERROR;

    Message msgRecv = (Message)serialIO.Read();

    switch (msgRecv) {
        case HELLO:{
            fprintf(stdout, "Connected\n");
            return HELLO;
            break;
        }

        case STRING: {
            char msgStr[100] = {0};
            int i = 0;
            bool timeout = false;
            while (i < sizeof(msgStr) && msgStr[i] != '\n' && msgStr[i] != '\r' && !timeout ) {
               if(serialIO.WaitForBytes(1, 10000)) {
                    msgStr[i] = serialIO.Read();
                    if (msgStr[i] == '\n' || msgStr[i] == '\r') {
                        msgStr[i + 1] = 0;
                    } else {
                        i++;
                    }
                }
                else
                    timeout = true;
            }
            if(timeout)
                fprintf(stderr, "String from timedout\n", msgStr);
            else
                fprintf(stdout, "String from ardunio: %s\n", msgStr);
            return STRING;
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
        case ERROR:{
            fprintf(stdout, "ERROR message from ardunio\n");
            return ERROR;
            break;
        }
        case RECEIVED:{
            fprintf(stdout, "RECEIVED message from ardunio\n");
            return RECEIVED;
            break;
        }
        default: {
            fprintf(stdout, "Unkown message from ardunio\n");
            break;
        }

    }
    return ERROR;
}

