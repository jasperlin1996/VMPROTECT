#include <iostream>
#include "../VMCore/include/opcodes.hpp"
#include "../VMCore/include/vmdebug.hpp"
#include <arpa/inet.h>
#include <bitset>

void printOptions()
{
    std::cout << "Options:\n"
                << "\t1. Execute a program on the VM\n"
                << "\t2. Step execution\n"
                << "\t3. Exit debugger and exit debug mode in the VM\n"
                << "\t4. Set a value in a register\n"
                << "\t5. Set a flag\n"
                << "\t6. Show the stack\n"
                << "\t7. Show the code data\n"
                << "\t8. Show the data buffer\n"
                << "\t9. Show all registers\n"
                << "\t10. Write to the code data"
                << std::endl;
    std::cout << "Choice: ";
}

void errorSend(int cliSocket)
{
    std::cout << "Failed send data in debug\n";
    close(cliSocket);
    exit(-1);
}

int main()
{
    std::cout << R"(
 _   ____  _______________ _____ _____ _____ _____ _____ 
| | | |  \/  || ___ \ ___ \  _  |_   _|  ___/  __ \_   _|
| | | | .  . || |_/ / |_/ / | | | | | | |__ | /  \/ | |  
| | | | |\/| ||  __/|    /| | | | | | |  __|| |     | |  
\ \_/ / |  | || |   | |\ \\ \_/ / | | | |___| \__/\ | |  
 \___/\_|  |_/\_|   \_| \_|\___/  \_/ \____/ \____/ \_/  
                                                         
                                                         
______ ___________ _   _ _____ _____  ___________        
|  _  \  ___| ___ \ | | |  __ \  __ \|  ___| ___ \       
| | | | |__ | |_/ / | | | |  \/ |  \/| |__ | |_/ /       
| | | |  __|| ___ \ | | | | __| | __ |  __||    /        
| |/ /| |___| |_/ / |_| | |_\ \ |_\ \| |___| |\ \        
|___/ \____/\____/ \___/ \____/\____/\____/\_| \_|       
                                                         
    )" << std::endl;
    std::cout << "version 0.1.200926.1921\n" << std::endl;

    int cliSocket = 0;
    struct sockaddr_in serv_addr;

    if ((cliSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        std::cout << "[ERROR] Failed socket creation\n"; 
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        std::cout << "[ERROR] Invalid address\n";
        return -1;
    }

    if (connect(cliSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        std::cout << "[ERROR] Connection failed\n";
        return -1;
    }

    bool connLoop = true;
    MESSAGE_TO_DEBUGGER msgToDebg;
    MESSAGE_FROM_DEBUGGER msgFromDebg;
    char bufferMSGtoDbg[PACKET_TO_DEBUGGER_SIZE];
    char bufferMSGfromDbg[PACKET_FROM_DEBUGGER_SIZE];
    int retValFromFunc;
    int cmdDBG;

    while(connLoop)
    {
        retValFromFunc = recvData(cliSocket, bufferMSGtoDbg, PACKET_TO_DEBUGGER_SIZE);
        if(retValFromFunc == RECIVE_ERROR)
        {
            std::cout << "[ERROR] Failed receive data\n";
            close(cliSocket);
            exit(-1);
        }
        deserializeMSG(&msgToDebg,bufferMSGtoDbg);
SHOWOPTIONS:
        printOptions();
        std::cin >> cmdDBG;
        switch(cmdDBG)
        {
            case 1:
                msgFromDebg.cmdFlag = CMD_RUN;
                serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                break;
            case 2:
                msgFromDebg.cmdFlag = CMD_STEP;
                serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                break;
            case 3:
                msgFromDebg.cmdFlag = CMD_EXIT;
                connLoop = false;
                serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                break;
            case 4:
                {
                    int option;
                    DWORD val;
                    int regNr;
                    std::cout << "Select a register to modify:\n"
                                << "\t1. PC\n"
                                << "\t2. SP\n"
                                << "\t3. Rx\n";
                    std::cout << "Choice: ";
                    std::cin >> option;
                    switch(option)
                    {
                        case 1:
                            std::cout << "Value (hex e.g. 1B or 1b): ";
                            std::cin >> std::hex >> val;
                            msgFromDebg.cmdFlag = CMD_SET_PC;
                            *(DWORD*) &msgFromDebg.buffer[0] = val;
                            serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                            retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                            if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                            break;
                        case 2:
                            std::cout << "Value (hex e.g. 1B or 1b): ";
                            std::cin >> std::hex >> val;
                            msgFromDebg.cmdFlag = CMD_SET_SP;
                            *(DWORD*) &msgFromDebg.buffer[0] = val;
                            serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                            retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                            if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                            break;
                        case 3:
                            std::cout << "register nr: ";
                            std::cin >> regNr;
                            std::cout << "Value (hex e.g. 1B or 1b): ";
                            std::cin >> std::hex >> val;
                            msgFromDebg.cmdFlag = CMD_SET_R;
                            msgFromDebg.buffer[0] = regNr + '0';
                            *(DWORD*) &msgFromDebg.buffer[1] = val;
                            serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                            retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                            if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                            break;
                        default:
                            std::cout << "Unkonown command!\n";
                            goto SHOWOPTIONS;
                    }
                }
                break;
            case 5:
                {
                    int option;
                    std::cout << "Select a flag to modify:\n"
                                << "\t1. ZF\n"
                                << "\t2. CF\n";
                    std::cout << "Choice: ";
                    std::cin >> option;
                    char val;
                    switch(option)
                    {
                        case 1:
                            {
                                std::cout << "Value: ";
                                std::cin >> val;
                                msgFromDebg.cmdFlag = CMD_SET_ZF;
                                msgFromDebg.buffer[0] = val;
                                serializeMSG(&msgFromDebg, bufferMSGfromDbg);
                                retValFromFunc = sendData(cliSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
                                if(retValFromFunc == SEND_ERROR) errorSend(cliSocket);
                            }
                            break;
                        case 2:
                            {
                                std::cout << "Value: ";
                                std::cin >> val;
                                msgFromDebg.cmdFlag = CMD_SET_CF;
                                msgFromDebg.buffer[0] = val;
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
                break;
            case 6:
                {
                    int numberDataToPrint = 0;
                    std::cout << "How many data to print: ";
                    std::cin >> numberDataToPrint;
                    if(numberDataToPrint > STACK_SIZE) numberDataToPrint = STACK_SIZE;
                    int sti = STACK_SIZE - 1;
                    while(sti >= (STACK_SIZE - numberDataToPrint))
                    {
                        std::cout << sti << ": " << std::bitset<32>(msgToDebg.stack[sti]) << std::endl;
                        --sti;
                    }
                    std::cout << "\n";
                    goto SHOWOPTIONS;
                }
                break;
            case 7:
                {
                    int numberDataToPrint = 0;
                    std::cout << "How many data to print: ";
                    std::cin >> numberDataToPrint;
                    if(numberDataToPrint > CODE_DATA_SIZE) numberDataToPrint = CODE_DATA_SIZE;
                    for(int i = 0; i < numberDataToPrint; i++)
                    {
                        std::cout << std::bitset<8>(msgToDebg.codeData[i]) << std::endl;
                    }
                    std::cout << "\n";
                    goto SHOWOPTIONS;
                }
                break;
            case 8:
                {
                    int numberDataToPrint = 0;
                    std::cout << "How many data to print: ";
                    std::cin >> numberDataToPrint;
                    if(numberDataToPrint > INPUT_BUFFER_SIZE) numberDataToPrint = INPUT_BUFFER_SIZE;
                    for(int i = 0; i < numberDataToPrint; i++)
                    {
                        std::cout << std::bitset<8>(msgToDebg.dataBuffer[i]) << std::endl;
                    }
                    std::cout << "\n";
                    goto SHOWOPTIONS;
                }
                break;
            case 9:
                std::cout << "PC: " << std::bitset<32>(msgToDebg.PC) << std::endl;
                std::cout << "SP: " << std::bitset<32>(msgToDebg.SP) << std::endl;
                for(int i = 0; i < 8; i++) 
                {
                    std::cout << "R[" << i << "]: " << std::bitset<32>(msgToDebg.R[i]) << std::endl;
                }
                std::cout << "ZF: " << std::bitset<8>(msgToDebg.ZF) << std::endl;
                std::cout << "CF: " << std::bitset<8>(msgToDebg.CF) << std::endl;
                goto SHOWOPTIONS;
                break;
            case 10:
                {
                    std::string toModify = "";
                    std::cout << "Data to modify (in bit, 1byte==8bit): ";
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

    close(cliSocket);
    return 0;
}