#include "../include/vmcpu.hpp"

void VMCPU::debug()
{
#ifndef VMTESTS
    bool debugLoop = true;
    
    #ifdef _WIN32_DEV_ENVIRONMENT
        //Initialize Winsock
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != NO_ERROR)
        {
            std::cout << "[ERROR " << iResult << "] WSAStartup failed" << std::endl;
            exit(iResult);;
        }
        //Create a SOCKET for listening for incoming connections request
        SOCKET serverSocket, debuggerSocket;
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET)
        {
            std::cout << "[ERROR " << WSAGetLastError() << "] Socket failed" << std::endl;
            WSACleanup();
            exit(WSAGetLastError());
        }

        sockaddr_in addrServer;
        addrServer.sin_family = AF_INET;
        InetPton(AF_INET, _T("127.0.0.1"), &addrServer.sin_addr.s_addr);
        addrServer.sin_port = htons(PORT);
        memset(&(addrServer.sin_zero), '\0', 8);

        //Bind socket
        if (bind(serverSocket, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR)
        {
            std::cout << "[ERROR " << WSAGetLastError() << "] Bind failed" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(WSAGetLastError());
        }

        debuggerSocket = accept(serverSocket, NULL, NULL);
        if (debuggerSocket == INVALID_SOCKET)
        {
            std::cout << "[ERROR " << WSAGetLastError() << "] Accept failed" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(WSAGetLastError());
        }

        closesocket(serverSocket);
    #else //_LINUX_DEV_ENVIRONMENT
        int serverSocket, debuggerSocket;
        struct sockaddr_in address;
        int opt = 1; 
        int addrlen = sizeof(address);

        if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
        { 
            std::cout << "[ERROR 101002] Socket failed in debug\n";
            exit(EXIT_FAILURE); 
        }

        if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
        { 
            std::cout << "[ERROR 101003] setsockopt in debug\n";
            exit(EXIT_FAILURE); 
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if(bind(serverSocket, (struct sockaddr *)&address, sizeof(address))<0) 
        { 
            std::cout << "[ERROR 101004] Bind failed in debug\n";
            exit(EXIT_FAILURE);
        } 
        if (listen(serverSocket, 3) < 0) 
        { 
            std::cout << "[ERROR 101005] Listen failed in debug\n";
            exit(EXIT_FAILURE);
        } 
        if ((debuggerSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
        { 
            std::cout << "[ERROR 101006] Accept failed in debug\n";
            exit(EXIT_FAILURE);
        }
    #endif

        VBYTE opcode;
        MESSAGE_TO_DEBUGGER msgToDebg;
        MESSAGE_FROM_DEBUGGER msgFromDebg;
        char bufferMSGtoDbg[PACKET_TO_DEBUGGER_SIZE];
        char bufferMSGfromDbg[PACKET_FROM_DEBUGGER_SIZE];
        int retValFromFunc;
        while(debugLoop)
        {
            memcpy(msgToDebg.R, REGS->R, 32);
            msgToDebg.PC = REGS->PC;
            msgToDebg.SP = REGS->SP;
            memcpy(msgToDebg.stack, AS->stack, STACK_SIZE);
            for(int asStI = 0; asStI < STACK_SIZE; asStI++) { msgToDebg.stack[asStI] = AS->stack[asStI]; }
            msgToDebg.ZF = REGS->ZF;
            msgToDebg.CF = REGS->CF;
            memcpy(msgToDebg.codeData, AS->codeData, CODE_DATA_SIZE);
            memcpy(msgToDebg.dataBuffer, AS->dataBuffer, INPUT_BUFFER_SIZE);
            serializeMSG(&msgToDebg, bufferMSGtoDbg);

            retValFromFunc = sendData(debuggerSocket, bufferMSGtoDbg, PACKET_TO_DEBUGGER_SIZE);
            if(retValFromFunc == SEND_ERROR)
            {
                std::cout << "[ERROR 101007] Failed send data in debug\n";
                #ifdef _WIN32_DEV_ENVIRONMENT
                    closesocket(debuggerSocket);
                    WSACleanup();
                #else //_LINUX_DEV_ENVIRONMENT
                    close(debuggerSocket);
                    close(serverSocket);
                #endif
                debugLoop = false;
                exit(-101007);
            }

            retValFromFunc = recvData(debuggerSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
            if(retValFromFunc == RECIVE_ERROR)
            {
                std::cout << "[ERROR 101008] Failed receive data in debug\n";
                #ifdef _WIN32_DEV_ENVIRONMENT
                    closesocket(debuggerSocket);
                    WSACleanup();
                #else //_LINUX_DEV_ENVIRONMENT
                    close(debuggerSocket);
                    close(serverSocket);
                #endif
                debugLoop = false;
                exit(-101008);
            }
            deserializeMSG(&msgFromDebg, bufferMSGfromDbg);

            switch(msgFromDebg.cmdFlag)
            {
                case CMD_RUN:
                    {
                        bool exit = false;
                        while(!exit)
                        {
                            opcode = AS->codeData[REGS->PC++];
                            exit = executer(opcode);
                        }
                    }
                    break;
                case CMD_STEP:
                    opcode = AS->codeData[REGS->PC++];
                    executer(opcode);
                    break;
                case CMD_EXIT:
                    debugLoop = false;
                    break;
                case CMD_SET_PC:
                    REGS->PC = *(VDWORD*) &msgFromDebg.buffer[0];
                    break;
                case CMD_SET_SP:
                    REGS->SP = *(VDWORD*) &msgFromDebg.buffer[0];
                    break;
                case CMD_SET_R:
                    {
                        int regNr = msgFromDebg.buffer[0] - '0';
                        REGS->R[regNr] = *(VDWORD*) &msgFromDebg.buffer[1];
                    }
                    break;
                case CMD_SET_ZF:
                    if(msgFromDebg.buffer[0] == '0'|| msgFromDebg.buffer[0] == '1') REGS->ZF = msgFromDebg.buffer[0];
                    break;
                case CMD_SET_CF:
                    if(msgFromDebg.buffer[0] == '0'|| msgFromDebg.buffer[0] == '1') REGS->CF = msgFromDebg.buffer[0];
                    break;
                case CMD_WRITE_MEM:
                    {
                        int VBYTEToWrite = msgFromDebg.buffer[0] - '0';
                        for(int i = 0; i < VBYTEToWrite; i++) { AS->codeData[REGS->PC++] = msgFromDebg.buffer[i + 1]; }
                    }
                    break;
                default:
                    std::cout << "[ERROR 101009] Unkonown command!\n";
            }
        }

    #ifdef _WIN32_DEV_ENVIRONMENT
        closesocket(debuggerSocket);
        WSACleanup();
    #else //_LINUX_DEV_ENVIRONMENT
        close(debuggerSocket);
        close(serverSocket);
    #endif
#endif
    return;
}