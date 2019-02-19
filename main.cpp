#include "Message.h"
#include <unistd.h>

#include "SerialIO/SerialIO.h"
#include "Joystick/Joystick.h"

bool isConnected = false;
SerialIO serialIO("/dev/ttyACM0", 115200);


Message getMessage();

int main() {
    Joystick joystick("/dev/input/js0");
    double servoScale;
    {
        JoystickEvent joystickEvent;
        servoScale = 100 / (double)joystickEvent.MAX_AXES_VALUE;
    }

    // Ensure that it was found and that we can use it
    if (!joystick.isFound())
    {
        printf("open failed.\n");
        exit(1);
    }

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

    while (true) {
        // Restrict rate
        usleep(1000);

        // Attempt to sample an event from the joystick
        JoystickEvent event;
        if (joystick.sample(&event))
        {
            if (event.isButton())
            {
                switch (event.number) {
                    case 0: {
                        if (event.value == 1) {
                            char msgToSend[] = {MOTOR, STOP};
                            serialIO.Send(msgToSend, sizeof(msgToSend));
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
            else if (event.isAxis())
            {
                switch (event.number) {
                    case 1: {
                        char msgToSend[3];
                        msgToSend[0] = MOTOR;

                        if (event.value == 0) {
                            msgToSend[1] = STOP;
                            serialIO.Send(msgToSend, 2);
                            break;
                        } else if (event.value<0) {
                            msgToSend[1] = FORWARD;
                        } else {
                            msgToSend[1] = REVERSE;
                        }

                        msgToSend[2] = 20;
                        serialIO.Send(msgToSend, sizeof(msgToSend));
                        break;
                    }
                    case 2: {
                        char msgToSend[3];
                        msgToSend[0] = SERVO;

                        if (event.value == 0){
                            msgToSend[1] = CENTER;
                            serialIO.Send(msgToSend, 2);
                            break;
                        } else if (event.value > 0) {
                            msgToSend[1] = RIGHT;
                        } else {
                            msgToSend[1] = LEFT;
                        }


                        short howFar = servoScale * abs(event.value);
                        msgToSend[2] = howFar;

                        serialIO.Send(msgToSend, sizeof(msgToSend));

                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
        if (serialIO.BytesQued()>0)
            getMessage();
    }
}

Message getMessage() {
    if (serialIO.BytesQued() <= 0)
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
                fprintf(stderr, "String from timedout, partial msg: %s\n", msgStr);
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

