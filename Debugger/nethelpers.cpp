#include "../VMCore/include/nethelpers.hpp"

int sendData(int socket, void *buffer, size_t length)
{
    char *ptr = (char*) buffer;
    int i;

    while (length > 0)
    {
        i = send(socket, ptr, length, 0);
        if (i < 0) return SEND_ERROR;
        else if(i == 0) return SEND_ZERO;
        ptr += i;
        length -= i;
    }
    return SEND_ALL_DATA;
}

int recvData(int socket, void *buffer, size_t length)
{
    char *ptr = (char*) buffer;
    int i;
    int iteration = 0;
    bool loopFinish = false;

    while (!loopFinish)
    {
        i = recv(socket, ptr, length, 0);
        if (i == -1) return RECIVE_ERROR;
        else if(i == 0)
        {
            if(iteration == 0) return RECIVE_ZERO;
            else loopFinish = true;
        }
        ptr += i;
        length -= i;
        ++iteration;
        if(length <= 0) loopFinish = true;
    }
    return i;
}

void serializeMSG(MESSAGE_TO_DEBUGGER *msgPacket, char *dataArray)
{
    DWORD *d1 = (DWORD *) dataArray;
    memcpy(d1, msgPacket->R, 8);
    d1 += DWORD(8);
    *d1 = msgPacket->PC; ++d1;
    *d1 = msgPacket->SP; ++d1;
    // memcpy(d1, msgPacket->stack, STACK_SIZE);
    for(DWORD i = 0; i < STACK_SIZE; i++)
    {
        *d1 = msgPacket->stack[i];
        d1 += DWORD(1);
    }
    // d1 += DWORD(STACK_SIZE);
    char *d2 = (char *) d1;
    *d2 = msgPacket->ZF; ++d2;
    *d2 = msgPacket->CF; ++d2;
    memcpy(d2, msgPacket->codeData, CODE_DATA_SIZE);
    d2 += CODE_DATA_SIZE;
    memcpy(d2, msgPacket->dataBuffer, INPUT_BUFFER_SIZE);
    d2 += INPUT_BUFFER_SIZE;
}

void deserializeMSG(MESSAGE_TO_DEBUGGER *msgPacket, char *dataArray)
{
    DWORD *d1 = (DWORD *) dataArray;
    memcpy(msgPacket->R, d1, 8);
    d1 += DWORD(8);
    msgPacket->PC = *d1; ++d1;
    msgPacket->SP = *d1; ++d1;
    // memcpy(msgPacket->stack, d1, STACK_SIZE);
    for(DWORD i = 0; i < STACK_SIZE; i++)
    {
        msgPacket->stack[i] = *d1; ++d1;
    }
    // d1 += DWORD(STACK_SIZE);

    char *d2 = (char *) d1;
    msgPacket->ZF = *d2; ++d2;
    msgPacket->CF = *d2; ++d2;
    memcpy(msgPacket->codeData, d2, CODE_DATA_SIZE);
    d2 += CODE_DATA_SIZE;
    memcpy(msgPacket->dataBuffer, d2, INPUT_BUFFER_SIZE);
    d2 += INPUT_BUFFER_SIZE;
}

void serializeMSG(MESSAGE_FROM_DEBUGGER *msgPacket, char *dataArray)
{
    int *d1 = (int *) dataArray;
    *d1 = msgPacket->cmdFlag; ++d1;

    char *d2 = (char *) d1;
    memcpy(d2, msgPacket->buffer, MSG_FROM_DBG_SIZE);
}

void deserializeMSG(MESSAGE_FROM_DEBUGGER *msgPacket, char *dataArray)
{
    int *d1 = (int *) dataArray;
    msgPacket->cmdFlag = *d1; ++d1;

    char *d2 = (char *) d1;
    memcpy(msgPacket->buffer, d2, MSG_FROM_DBG_SIZE);
}