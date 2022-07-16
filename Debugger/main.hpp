#ifndef DEBUGGER_MAIN_HPP
#define DEBUGGER_MAIN_HPP

#include <iostream>
#include <string>
#include "../SharedCode/opcodes.hpp"
#include "../VMCore/include/vmcpu.hpp"
#include "../SharedCode/vmdebug.hpp"

#ifdef _WIN32_DEV_ENVIRONMENT
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string> 

    // Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
    #pragma comment (lib, "Ws2_32.lib")
    #pragma comment (lib, "Mswsock.lib")
    #pragma comment (lib, "AdvApi32.lib")
#endif

#ifdef _LINUX_DEV_ENVIRONMENT
    #include <arpa/inet.h>
#endif

#include <bitset>

#include "../SharedCode/global.hpp"

enum class DEBUG_OPTIONS { 
    EXEC = 1, 
    STEP,
    EXIT_DBG,
    SET_VAL,
    SET_FLAG,
    SHOW_STACK,
    SHOW_CODE_DATA,
    SHOW_DATA_BUFFER,
    SHOW_REGS,
    WRITE_CODE
};


/* *****************  FUNCTIONS *****************  */
void printIntro();
void printOptions();
void printRegToModifyOptions();
void printFlagToModifyOptions();

void showData(MESSAGE_TO_DEBUGGER *, DEBUG_OPTIONS);

void errorSend(int);
void conn(int);
void sendCmd(int, int);
void setRegValue(int, int, VDWORD, int);

#ifdef _WIN32_DEV_ENVIRONMENT
    int setWin32Connection();
#else _LINUX_DEV_ENVIRONMENT
    int setLinuxConnection();
#endif
/* **********************************************  */

#endif