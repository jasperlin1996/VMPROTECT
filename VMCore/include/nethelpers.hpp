#ifndef VM_NET_HELPERS_H
#define VM_NET_HELPERS_H

#include "./vmcpu.hpp"

#include <string.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h>

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
    DWORD R[8];
    DWORD PC;
    DWORD SP;
    DWORD stack[STACK_SIZE];
    unsigned char ZF;
    unsigned char CF;
    BYTE codeData[CODE_DATA_SIZE];
    BYTE dataBuffer[INPUT_BUFFER_SIZE];
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