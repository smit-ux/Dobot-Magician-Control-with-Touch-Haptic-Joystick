#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with Ws2_32.lib

#if defined(WIN32) || defined(_WIN32)
# include <windows.h>
# include <conio.h>
#else
#include<time.h>
#include "conio.h"
#define FALSE 0
#define TRUE 1
#endif

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>



constexpr auto MAX_INPUT_DOF = 6;
constexpr auto MAX_OUTPUT_DOF = 6;

static int gNumMotors = 0;
static int gNumEncoders = 0;

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with Ws2_32.lib

SOCKET setupSocket(const char* server_ip, int server_port) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d\n", WSAGetLastError());
        WSACleanup();
        return INVALID_SOCKET;
    }

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection Failed\n");
        closesocket(sock);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return sock;
}

void sendDataOverTCP(SOCKET sock, const char* data) {
    send(sock, data, strlen(data), 0);
    printf("Data sent to server: %s\n", data);
}

void PrintHelp()
{
    static const char help[] = { \
"CommandMotorDAC Help\n\
---\n\
P: Prints device state\n\
C: Continuously prints device state\n\
H: Prints help menu\n\
Q: Quits the program\n\
---" };

    printf("\n%s\n", help);
}

HDSchedulerHandle gCallbackHandle = HD_INVALID_HANDLE;




/*****************************************************************************
 Servo Scheduler Callback 
*****************************************************************************/
HDCallbackCode HDCALLBACK ServoSchedulerCallback(void* pUserData)
{
    HDErrorInfo error;

    hdBeginFrame(hdGetCurrentDevice());

    hdEndFrame(hdGetCurrentDevice());

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    return HD_CALLBACK_CONTINUE;
}


/* Synchronization structure. */
typedef struct
{
    HDlong encoder_values[MAX_INPUT_DOF];
    HDlong motor_dac_values[MAX_OUTPUT_DOF];
    hduVector3Dd position;
    HDint buttons[1];
    HDdouble velocity[3];
    HDdouble angular_velocity[3];
    HDdouble joint_angles[3];
    HDdouble gimble_angles[3];
    HDdouble force[3];
    HDdouble torque[3];
    HDdouble joint_torque[3];
    HDdouble gimble_torque[3];
} DeviceStateStruct;

/*****************************************************************************
 Callback that retrieves state.
*****************************************************************************/
HDCallbackCode HDCALLBACK GetDeviceStateCallback(void* pUserData)
{
    DeviceStateStruct* pState = (DeviceStateStruct*)pUserData;

    hdGetLongv(HD_CURRENT_ENCODER_VALUES, pState->encoder_values);
    hdGetLongv(HD_CURRENT_MOTOR_DAC_VALUES, pState->motor_dac_values);
    hdGetDoublev(HD_CURRENT_POSITION, pState->position);
    hdGetIntegerv(HD_CURRENT_BUTTONS, pState->buttons);
    hdGetDoublev(HD_CURRENT_VELOCITY, pState->velocity);
    hdGetDoublev(HD_CURRENT_ANGULAR_VELOCITY, pState->angular_velocity);
    hdGetDoublev(HD_CURRENT_JOINT_ANGLES, pState->joint_angles);
    hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, pState->gimble_angles);
    hdGetDoublev(HD_CURRENT_FORCE, pState->force);
    hdGetDoublev(HD_CURRENT_TORQUE, pState->torque);
    hdGetDoublev(HD_CURRENT_JOINT_TORQUE, pState->joint_torque);
    hdGetDoublev(HD_CURRENT_GIMBAL_TORQUE, pState->gimble_torque);
    return HD_CALLBACK_DONE;
}

/*****************************************************************************
 Callback that retrieves state.
*****************************************************************************/
void PrintDeviceState(SOCKET sock, HDboolean bContinuous) {
    int i;
    DeviceStateStruct state;
    char data[1024];

    memset(&state, 0, sizeof(DeviceStateStruct));

    do {
        hdScheduleSynchronous(GetDeviceStateCallback, &state, HD_DEFAULT_SCHEDULER_PRIORITY);

        printf("\n");

        printf("Motor DAC Values:");
        for (i = 0; i < gNumMotors; i++) {
            printf(" %d", state.motor_dac_values[i]);
        }
        printf("\n");

        printf("Encoder Values:");
        for (i = 0; i < gNumEncoders; i++) {
            printf(" %d", state.encoder_values[i]);
        }
        printf("\n");

        printf("Position(mm):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.position[i]);
        }
        printf("\n");

        printf("Buttons Values:");
        for (i = 0; i < 1; i++) {
            printf(" %d", state.buttons[i]);
        }
        printf("\n");

        printf("Current Velocity(mm/s):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.velocity[i]);
        }
        printf("\n");

        printf("Current Angular Velocity(rad/s):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.angular_velocity[i]);
        }
        printf("\n");

        printf("Current Joint Angles(rad):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.joint_angles[i]);
        }
        printf("\n");

        printf("Current Gimble Angles(rad):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.gimble_angles[i]);
        }
        printf("\n");

        printf("Current FORCE(N):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.force[i]);
        }
        printf("\n");

        printf("Current Torque(mNm):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.torque[i]);
        }
        printf("\n");

        printf("Current Joint Torque(mNm):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.joint_torque[i]);
        }
        printf("\n");

        printf("Current Gimble Torque(mNm):");
        for (i = 0; i < 3; i++) {
            printf(" %f", state.gimble_torque[i]);
        }
        printf("\n");

        sprintf(data, "Position: %f %f %f, Buttons: %d", state.position[0], state.position[1], state.position[2], state.buttons[0]);

        sendDataOverTCP(sock, data);

        printf("\n");
        if (bContinuous) {
            Sleep(100);
        }

    } while (!_kbhit() && bContinuous);
}
/******************************************************************************
 The main loop of execution.  Detects and interprets keypresses.  Monitors and
 initiates error recovery if necessary.
******************************************************************************/
void mainLoop(SOCKET sock) {
    int keypress;

    while (TRUE) {
        if (_kbhit()) {
            keypress = getch();
            keypress = toupper(keypress);

            switch (keypress) {
            case 'P': PrintDeviceState(sock, FALSE); break;
            case 'C': PrintDeviceState(sock, TRUE); break;
            case 'Q': return;
            default: PrintHelp(); break;
            }
        }

        if (!hdWaitForCompletion(gCallbackHandle, HD_WAIT_CHECK_STATUS)) {
            fprintf(stderr, "\nThe main scheduler callback has exited\n");
            fprintf(stderr, "\nPress any key to quit.\n");
            getch();
            return;
        }
    }
}


/*******************************************************************************
 Main function.
*******************************************************************************/
int main(int argc, char* argv[])
{
    HDErrorInfo error;
    HDstring model;
    HDboolean bDone = FALSE;
    const char* server_ip = "127.0.0.1";
    int server_port = 65432;

    SOCKET sock = setupSocket(server_ip, server_port);
    if (sock == INVALID_SOCKET) {
        printf("Failed to setup socket. Exiting.\n");
        return 1;
    }

    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();

        return -1;
    }

    model = hdGetString(HD_DEVICE_MODEL_TYPE);
    printf("Initialized: %s\n", model);

    hdGetIntegerv(HD_OUTPUT_DOF, &gNumMotors);
    hdGetIntegerv(HD_INPUT_DOF, &gNumEncoders);

#if defined(linux)
    nocbreak();
#endif

    /* Schedule the haptic callback function for continuously monitoring the
       button state and rendering the anchored spring force */
    gCallbackHandle = hdScheduleAsynchronous(
        ServoSchedulerCallback, 0, HD_MAX_SCHEDULER_PRIORITY);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to schedule servoloop callback");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();

        hdDisableDevice(hHD);
        return -1;
    }

    hdEnable(HD_FORCE_OUTPUT);

    /* Start the haptic rendering loop */
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start servoloop");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();

        hdDisableDevice(hHD);
        return -1;
    }

    PrintHelp();

    /* Start the main application loop */
    mainLoop(sock);

    hdStopScheduler();
    hdUnschedule(gCallbackHandle);
    hdDisableDevice(hHD);

    closesocket(sock);
    WSACleanup();

    return 0;
}


