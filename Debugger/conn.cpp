#include "main.hpp"

void conn(int cliSocket)
{
    bool connLoop = true;
    MESSAGE_TO_DEBUGGER msgToDebg;
    MESSAGE_FROM_DEBUGGER msgFromDebg;
    char bufferMSGtoDbg[PACKET_TO_DEBUGGER_SIZE];
    char bufferMSGfromDbg[PACKET_FROM_DEBUGGER_SIZE];
    int retValFromFunc;
    DEBUG_OPTIONS cmdDBG;

    while(connLoop)
    {
        retValFromFunc = recvData(cliSocket, bufferMSGtoDbg, PACKET_TO_DEBUGGER_SIZE);
        if(retValFromFunc == RECIVE_ERROR)
        {
            std::cout << "[ERROR] Failed receive data\n";
            #ifdef  _LINUX_DEV_ENVIRONMENT
                close(cliSocket);
            #else //_WIN32_DEV_ENVIRONMENT
                closesocket(cliSocket);
                WSACleanup();
            #endif
            exit(-1);
        }
        deserializeMSG(&msgToDebg,bufferMSGtoDbg);
SHOWOPTIONS:
        printOptions();
        int temp_cmdDBG;
        std::cin >> temp_cmdDBG;
        cmdDBG = static_cast<DEBUG_OPTIONS>(temp_cmdDBG);
        switch(cmdDBG)
        {
            case DEBUG_OPTIONS::EXEC:
                sendCmd(cliSocket, CMD_RUN);
                break;
            case DEBUG_OPTIONS::STEP:
                sendCmd(cliSocket, CMD_STEP);
                break;
            case DEBUG_OPTIONS::EXIT_DBG:
                connLoop = false;
                sendCmd(cliSocket, CMD_EXIT);
                break;
            case DEBUG_OPTIONS::SET_VAL:
                {
                    int option;
                    VDWORD val;
                    printRegToModifyOptions();
                    std::cin >> option;
                    std::cout << "Value (hex e.g. 1B or 1b): ";
                    std::cin >> std::hex >> val;
                    switch(option)
                    {
                        case 1:
                            setRegValue(cliSocket, CMD_SET_PC, val, -1);
                            break;
                        case 2:
                            setRegValue(cliSocket, CMD_SET_SP, val, -1);
                            break;
                        case 3:
                            int regNr;
                            std::cout << "register nr: ";
                            std::cin >> regNr;
                            setRegValue(cliSocket, CMD_SET_SP, val, regNr);
                            break;
                        default:
                            std::cout << "Unkonown command!\n";
                            goto SHOWOPTIONS;
                    }
                }
                break;
            case DEBUG_OPTIONS::SET_FLAG:
                {
                    int option;
                    char val;
                    printFlagToModifyOptions();
                    std::cin >> option;
                    std::cout << "Value: ";
                    std::cin >> val;
                    switch(option)
                    {
                        case 1:
                            setRegValue(cliSocket, CMD_SET_ZF, val, -1);
                            break;
                        case 2:
                            setRegValue(cliSocket, CMD_SET_CF, val, -1);
                            break;
                        default:
                            std::cout << "Unkonown command!\n";
                            goto SHOWOPTIONS;
                    }
                }
                break;
            case DEBUG_OPTIONS::SHOW_STACK:
                {
                    showData(&msgToDebg, cmdDBG);
                    goto SHOWOPTIONS;
                }
                break;
            case DEBUG_OPTIONS::SHOW_CODE_DATA:
                {
                    showData(&msgToDebg, cmdDBG);
                    goto SHOWOPTIONS;
                }
                break;
            case DEBUG_OPTIONS::SHOW_DATA_BUFFER:
                {
                    showData(&msgToDebg, cmdDBG);
                    goto SHOWOPTIONS;
                }
                break;
            case DEBUG_OPTIONS::SHOW_REGS:
                std::cout << "PC: " << std::bitset<32>(msgToDebg.PC) << std::endl;
                std::cout << "SP: " << std::bitset<32>(msgToDebg.SP) << std::endl;
                for(int i = 0; i < 8; i++) std::cout << "R[" << i << "]: " << std::bitset<32>(msgToDebg.R[i]) << std::endl;
                std::cout << "ZF: " << std::bitset<8>(msgToDebg.ZF) << std::endl;
                std::cout << "CF: " << std::bitset<8>(msgToDebg.CF) << std::endl;
                goto SHOWOPTIONS;
                break;
            case DEBUG_OPTIONS::WRITE_CODE:
                {
                    std::string toModify = "";
                    std::cout << "Data to modify (in bit, 1VBYTE==8bit): ";
                    std::cin >> toModify;
                    if((toModify.length()/8) > MSG_FROM_DBG_SIZE) 
                    {
                        toModify = toModify.substr(0, MSG_FROM_DBG_SIZE);
                    }
                    msgFromDebg.buffer[0] = (toModify.length()/8) + '0';
                    msgFromDebg.cmdFlag = CMD_WRITE_MEM;
                    int counter = 1;
                    for(int k = 0; k < toModify.length(); k+=8) 
                    {
                        std::string binary_num = toModify.substr(k, 8); 
                        msgFromDebg.buffer[counter++] = std::stol(binary_num, nullptr, 2); 
                    }
                    serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                    retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                    if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                }
                break;
            default:
                std::cout << "Unkonown command!\n";
                goto SHOWOPTIONS;
        }
    }

#ifdef  _LINUX_DEV_ENVIRONMENT
    close(cliSocket);
#else //_WIN32_DEV_ENVIRONMENT
    closesocket(cliSocket);
    WSACleanup();
#endif
}

void sendCmd(int cliSocket, int cmdNumber)
{
    int retValFromFunc;
    MESSAGE_FROM_DEBUGGER msgFromDebg;
    char bufferMSGfromDbg[PACKET_FROM_DEBUGGER_SIZE];

    msgFromDebg.cmdFlag = CMD_RUN;
    serializeMSG(&msgFromDebg, bufferMSGfromDbg);
    retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
    if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
    return;
}

void setRegValue(int cliSocket, int cmdNumber, VDWORD val, int regNr)
{
    int retValFromFunc;
    MESSAGE_FROM_DEBUGGER msgFromDebg;
    char bufferMSGfromDbg[PACKET_FROM_DEBUGGER_SIZE];

    msgFromDebg.cmdFlag = cmdNumber;
    if(regNr == -1)
    {
        *(VDWORD*) &msgFromDebg.buffer[0] = val;
    }
    else
    {
        msgFromDebg.buffer[0] = regNr + '0';
        *(VDWORD*) &msgFromDebg.buffer[1] = val;
    }
    serializeMSG(&msgFromDebg, bufferMSGfromDbg);
    retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
    if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
    return;
}

void showData(MESSAGE_TO_DEBUGGER *msgToDebg, DEBUG_OPTIONS option)
{
    int numberDataToPrint = 0;
    std::cout << "How many data to print: ";
    std::cin >> numberDataToPrint;

    switch(option)
    {
        case DEBUG_OPTIONS::SHOW_STACK:
            {
                if(numberDataToPrint > STACK_SIZE) numberDataToPrint = STACK_SIZE;
                int sti = STACK_SIZE - 1;
                while(sti >= (STACK_SIZE - numberDataToPrint))
                {
                    std::cout << sti << ": " << std::bitset<32>(msgToDebg->stack[sti]) << std::endl;
                    --sti;
                }
            }
            break;
        case DEBUG_OPTIONS::SHOW_CODE_DATA:
            {
                if(numberDataToPrint > CODE_DATA_SIZE) numberDataToPrint = CODE_DATA_SIZE;
                for(int i = 0; i < numberDataToPrint; i++) std::cout << std::bitset<8>(msgToDebg->codeData[i]) << std::endl;
            }
            break;
        case DEBUG_OPTIONS::SHOW_DATA_BUFFER:
            {
                if(numberDataToPrint > INPUT_BUFFER_SIZE) numberDataToPrint = INPUT_BUFFER_SIZE;
                for(int i = 0; i < numberDataToPrint; i++) std::cout << std::bitset<8>(msgToDebg->dataBuffer[i]) << std::endl;
            }
            break;
        default:
                std::cout << "Unkonown command!\n";
    }
    std::cout << "\n";
}