#ifndef VM_NET_HELPERS_H
#define VM_NET_HELPERS_H

#include "datatypes.hpp"
#include "memsize.hpp"
#include "global.hpp"

#include <string.h>

#ifdef _LINUX_DEV_ENVIRONMENT
    #include <unistd.h> 
    #include <sys/socket.h> 
    #include <netinet/in.h>
#else //_WIN32_DEV_ENVIRONMENT
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <tchar.h>

    // Need to link with Ws2_32.lib
    #pragma comment (lib, "Ws2_32.lib")
#endif

#define SEND_ALL_DATA 1
#define SEND_ZERO 0
#define SEND_ERROR -1
#define RECIVE_ZERO 0
#define RECIVE_ERROR -1

int sendData(int, void *, size_t);
int recvData(int, void *, size_t);

#define PACKET_TO_DEBUGGER_SIZE sizeof(MESSAGE_TO_DEBUGGER)
#define PACKET_FROM_DEBUGGER_SIZE sizeof(MESSAGE_FROM_DEBUGGER)

struct MESSAGE_TO_DEBUGGER
{
    VDWORD R[8];
    VDWORD PC;
    VDWORD SP;
    VDWORD stack[STACK_SIZE];
    unsigned char ZF;
    unsigned char CF;
    VBYTE codeData[CODE_DATA_SIZE];
    VBYTE dataBuffer[INPUT_BUFFER_SIZE];
};

#define CMD_RUN 100
#define CMD_STEP 101
#define CMD_EXIT 102
#define CMD_SET_PC 103
#define CMD_SET_SP 104
#define CMD_SET_R 105
#define CMD_SET_ZF 106
#define CMD_SET_CF 107
#define CMD_WRITE_MEM 108

#define MSG_FROM_DBG_SIZE 20
struct MESSAGE_FROM_DEBUGGER
{
    int cmdFlag;
    char buffer[MSG_FROM_DBG_SIZE];
};

void serializeMSG(MESSAGE_TO_DEBUGGER *, char *);
void deserializeMSG(MESSAGE_TO_DEBUGGER *, char *);
void serializeMSG(MESSAGE_FROM_DEBUGGER *, char *);
void deserializeMSG(MESSAGE_FROM_DEBUGGER *, char *);

#endif