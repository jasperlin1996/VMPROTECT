#include "../include/vmcpu.hpp"

//#define V_DEBUG
//#include <bitset>

VMCPU::VMCPU()
{
    AS = (PADDRESS_SPACE) new ADDRESS_SPACE;
    REGS = (PREGISTERSS) new REGISTERSS;

    memset(AS->codeData, 0, sizeof(AS->codeData));

    REGS->PC = 0;
    REGS->SP = sizeof(AS->stack) / sizeof(DWORD);
}

VMCPU::~VMCPU()
{
    delete AS;
    delete REGS;
}

bool VMCPU::loadCode(BYTE *mcode, int mcsize)
{
    memset(AS->codeData, 0, CODE_DATA_SIZE*sizeof(*(AS->codeData)));
    memset(AS->stack, 0, STACK_SIZE*sizeof(*(AS->stack)));
    memset(AS->dataBuffer, 0, INPUT_BUFFER_SIZE*sizeof(*(AS->dataBuffer)));
    if((unsigned) (mcsize) > (sizeof(AS->codeData) / sizeof(AS->codeData[0]))) 
    {
        std::cout << "[ERROR 101001] TOO BIG A CODE TO EXECUTE!\n";
        return false;
    }
    memcpy(AS->codeData, mcode, mcsize);
    for(int i = 0; i < 8; i++)
    {
        REGS->R[i] = (DWORD) 0;
    }
    REGS->CF = 0;
    REGS->ZF = 0;
    return true;
}

void VMCPU::vmPrint(BYTE s)
{
    std::cout << s;
}

void VMCPU::vmPrintHX(DWORD hx)
{
    std::cout << std::hex << hx;
}

void VMCPU::vmPrintN(BYTE s)
{
    std::cout << s << std::endl;
}

void VMCPU::vmPrintHXN(DWORD hx)
{
    std::cout << std::hex << hx << std::endl;
}

void VMCPU::debug()
{
#ifndef VMTESTS
    bool debugLoop = true;

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

    BYTE opcode;
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
            close(debuggerSocket);
            close(serverSocket);
            debugLoop = false;
            exit(-101007);
        }

        retValFromFunc = recvData(debuggerSocket, bufferMSGfromDbg, PACKET_FROM_DEBUGGER_SIZE);
        if(retValFromFunc == RECIVE_ERROR)
        {
            std::cout << "[ERROR 101008] Failed receive data in debug\n";
            close(debuggerSocket);
            close(serverSocket);
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
                REGS->PC = *(DWORD*) &msgFromDebg.buffer[0];
                break;
            case CMD_SET_SP:
                REGS->SP = *(DWORD*) &msgFromDebg.buffer[0];
                break;
            case CMD_SET_R:
                {
                    int regNr = msgFromDebg.buffer[0] - '0';
                    REGS->R[regNr] = *(DWORD*) &msgFromDebg.buffer[1];
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
                    int byteToWrite = msgFromDebg.buffer[0] - '0';
                    for(int i = 0; i < byteToWrite; i++) { AS->codeData[REGS->PC++] = msgFromDebg.buffer[i + 1]; }
                }
                break;
            default:
                std::cout << "[ERROR 101009] Unkonown command!\n";
        }
    }
    close(debuggerSocket);
    close(serverSocket);
#endif
    return;
}

void VMCPU::run()
{
    bool exit = false;
    BYTE opcode;

    while(!exit)
    {
        opcode = AS->codeData[REGS->PC++];
        exit = executer(opcode);
    }
    return;
}

int VMCPU::executer(BYTE opcode)
{
    int valToReturn = 0;

    BYTE bTmp_0, bTmp_1, bTmp_2;
    WORD wTmp_0, wTmp_1;
    DWORD dTmp_0, dTmp_1, dTmp_2;

    switch(opcode)
    {
        /* NOP */
        case 0x56:
        case 0x6d:
        case NOP:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] NOP" << std::endl;
            #endif
            opcode+=20;
            break;
        /* EE - end of code */
        case EE:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] EE" << std::endl;
            #endif
            valToReturn = 1;
            break;
        /*  ********************************
                        MOV
            ******************************** 
        */
        /*
            MOV - move from register to register
            01 02 05 => MOV R2, R5
            01 00 00 => MOV R0, R0
        */
        case MOV:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOV" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            bTmp_1 = AS->codeData[REGS->PC++];
            #ifdef V_DEBUG
                std::cout << std::hex << REGS->R[bTmp_0] << std::endl;
                std::cout << std::hex << REGS->R[bTmp_1] << std::endl;
                // std::cout << std::hex << bTmp_0 << "\n";
                // std::cout << std::hex << bTmp_1 << "\n";
                // std::cout << std::bitset<8>(bTmp_0) << std::endl;
                // std::cout << std::bitset<8>(bTmp_1) << std::endl;
            #endif
            if((bTmp_0 >= 0 && bTmp_0 <= 7) && (bTmp_1 >= 0 && bTmp_1 <= 7))
            {
                REGS->R[bTmp_0] = REGS->R[bTmp_1];
            }
            else goto EXCEPTION;
            break;
        /* 
            MOVMB - move and extend byte from memory to register 
            02 03 04 01 => MOVMB R3, 0104
        */
        case MOVMB:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVMB" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            if(wTmp_0 >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->PC += 2;
            REGS->R[bTmp_0] = 0;
            *(BYTE*) &REGS->R[bTmp_0] = AS->codeData[wTmp_0];
            break;  
        /* 
            MOVMW - move and extend word from memory to register 
            03 03 04 01 => MOVMW R3, 0104
        */
        case MOVMW:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVMW" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            if(wTmp_0 >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->PC += 2;
            *(WORD*) &REGS->R[bTmp_0] = *(WORD*) &AS->codeData[wTmp_0];
            break;  
        /* 
            MOVB - move and extend byte to register 
            04 02 43 => MOVB R2, 43
        */
        case MOVB:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVB" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            REGS->R[bTmp_0] = 0;
            *(BYTE *) &REGS->R[bTmp_0] = AS->codeData[REGS->PC++];
            break; 
        /* 
            MOVW - move and extend word to register 
            05 01 15 28 => MOVW R1, 2815
        */
        case MOVW:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVW" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            *(WORD*) REGS->R[bTmp_0] = *(WORD *) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            break; 
        /* 
            MOVBM - move byte from register to memory location 
            06 04 43 13 => MOVBM 1343, R4
        */
        case MOVBM:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVBM" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            if(wTmp_0 >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->PC += 2;
            AS->codeData[wTmp_0] = *(BYTE*) &REGS->R[bTmp_0];
            break;  
        /* 
            MOVWM - move word from register to memory location 
            07 04 43 13 => MOVWM 1343, R4
        */
        case MOVWM:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVWM" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            if(wTmp_0 >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->PC += 2;
            *(WORD*) &AS->codeData[wTmp_0] = REGS->R[bTmp_0];
            break;
        /* 
            MOVMRB - move and extend byte from memory to register
                    get addr from register
            08 02 01 => MOVMRB R2, R1
        */
        case MOVMRB:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVMRB" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            if(REGS->R[bTmp_1] >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->R[bTmp_0] = 0;
            *(BYTE*) &REGS->R[bTmp_0] = AS->codeData[REGS->R[bTmp_1]];
            break;
        /* 
            MOVMRW - move and extend word from memory to register
                    get addr from register
            09 02 01 => MOVMRW R2, R1
        */
        case MOVMRW:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVMRW" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            if(REGS->R[bTmp_1] >= sizeof(AS->codeData)) goto EXCEPTION;
            *(WORD*) REGS->R[bTmp_0] = *(WORD*) &AS->codeData[REGS->R[bTmp_1]];
            break;
        /* 
            MOVMD - move double word from memory to register 
            0A 03 04 01 => MOVMW R3, 0104
        */
        case MOVMD:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVMD" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            dTmp_0 = *(DWORD*) &AS->codeData[REGS->PC];
            if(dTmp_0 >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->PC += 2;
            REGS->R[bTmp_0] = *(DWORD*) &AS->codeData[dTmp_0];
            break;  
        /* 
            MOVD - move value to register 
            0B 01 00 00 15 28 => MOVW R1, 2815
        */
        case MOVD:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVD" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            REGS->R[bTmp_0] = *(DWORD *) &AS->codeData[REGS->PC];
            REGS->PC += 4;
            break;  
        /* 
            MOVDM - move double word from register to memory location 
            0C 04 43 13 => MOVWM 1343, R4
        */
        case MOVDM:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVDM" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            if(wTmp_0 >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->PC += 2;
            *(DWORD*) &AS->codeData[wTmp_0] = REGS->R[bTmp_0];
            break;
        /* 
            MOVMRD - move double word from memory to register
                    get addr from register
            0D 02 01 => MOVMRW R2, R1
        */
        case MOVMRD:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] MOVMRD" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            if(REGS->R[bTmp_1] >= sizeof(AS->codeData)) goto EXCEPTION;
            REGS->R[bTmp_0] = *(DWORD*) &AS->codeData[REGS->R[bTmp_1]];
            break;
        /*  ********************************
                        JUMP
            ********************************
        */
        /*
            JMP - unconditional jump
            20 15 00 => JMP 0015
        */
        case JMP:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] JMP" << std::endl;
            #endif
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            if(wTmp_0 > sizeof(AS->codeData)) goto EXCEPTION;
            REGS->PC = wTmp_0;
            break; 
        /*
            JZ - jump if equal
            21 15 00 => JZ 0015
        */
        case JZ:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] JZ" << std::endl;
            #endif
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            if(wTmp_0 > sizeof(AS->codeData)) goto EXCEPTION;
            if(REGS->ZF) REGS->PC = wTmp_0;
            break;
        /*
            JNZ - jump if not equal
            22 15 00 => JNZ 0015
        */
        case JNZ:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] JNZ" << std::endl;
            #endif
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            if(wTmp_0 > sizeof(AS->codeData)) goto EXCEPTION;
            if(!REGS->ZF) REGS->PC = wTmp_0;
            break;
        /*
            JAE - jump if above or equal
            23 15 00 => JAE 0015
        */
        case JAE:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] JAE" << std::endl;
            #endif
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            if(wTmp_0 > sizeof(AS->codeData)) goto EXCEPTION;
            if(REGS->ZF || !REGS->CF) REGS->PC = wTmp_0;
            break;
        /*
            JBE - jump if below or equal
            24 15 00 => JBE 0015
        */
        case JBE:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] JBE" << std::endl;
            #endif
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            if(wTmp_0 > sizeof(AS->codeData)) goto EXCEPTION;
            if(REGS->ZF || REGS->CF) REGS->PC = wTmp_0;
            break;
        /*
            JB - jump if below
            25 15 00 => JB 0015
        */
        case JB:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] JB" << std::endl;
            #endif
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            if(wTmp_0 > sizeof(AS->codeData)) goto EXCEPTION;
            if(!REGS->ZF && REGS->CF) REGS->PC = wTmp_0;
            break;
        /*
            JA - jump if above
            26 15 00 => JA 0015
        */
        case JA:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] JA" << std::endl;
            #endif
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            if(wTmp_0 > sizeof(AS->codeData)) goto EXCEPTION;
            if(!REGS->ZF && !REGS->CF) REGS->PC = wTmp_0;
            break;
        /*  ********************************
                        ARITHMETIC
            ********************************
        */
        /*
            ADVR - Add word value to register
            30 02 10 00 => ADVR R2, 10
        */
        case ADVR:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] ADVR" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            wTmp_1 = REGS->R[bTmp_0] + wTmp_0;          
            if(wTmp_1 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(wTmp_1 < REGS->R[bTmp_0]) REGS->CF = 1;
            else REGS->CF = 0;
            *(WORD *) &REGS->R[bTmp_0] = wTmp_1;
            break;
        /*
            ADRR - Add two registers
                    and save result in first
            31 02 01 => ADRR R2, R1
        */
        case ADRR:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] ADRR" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            dTmp_0 = REGS->R[bTmp_0];
            dTmp_1 = REGS->R[bTmp_1];
            dTmp_2 = dTmp_0 + dTmp_1;
            REGS->R[bTmp_0] = dTmp_2;
            if(dTmp_2 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(dTmp_2 < dTmp_0) REGS->CF = 1;
            else REGS->CF = 0;
            break;
        /*
            ADRRL - Add two registers (low byte)
                    and save result in first
            32 02 01 => ADRR R2, R1
        */
        case ADRRL:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] ADRRL" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            bTmp_2 = *(BYTE *) &REGS->R[bTmp_0];
            bTmp_1 = *(BYTE *) &REGS->R[bTmp_1];
            bTmp_1 += bTmp_2;
            *(BYTE *) &REGS->R[bTmp_0] = bTmp_1;
            if(bTmp_1 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(bTmp_1 < bTmp_2) REGS->CF = 1;
            else REGS->CF = 0;
            break;
        /*
            SUBVR - Substract word value from register
            33 02 10 00 => SUBVR R2, 10
        */
        case SUBVR:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] SUBVR" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            wTmp_0 = *(WORD*) &AS->codeData[REGS->PC];
            REGS->PC += 2;
            wTmp_1 = REGS->R[bTmp_0] - wTmp_0;          
            if(wTmp_1 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(wTmp_1 > REGS->R[bTmp_0]) REGS->CF = 1;
            else REGS->CF = 0;
            *(WORD *) &REGS->R[bTmp_0] = wTmp_1;
            break;
        /*
            SUBRR - Substract two registers
                    and save result in first
            34 02 01 => SUBRR R2, R1
        */
        case SUBRR:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] SUBRR" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            dTmp_0 = REGS->R[bTmp_0];
            dTmp_1 = REGS->R[bTmp_1];
            dTmp_2 = dTmp_0 - dTmp_1;
            REGS->R[bTmp_0] = dTmp_2;
            if(dTmp_2 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(dTmp_2 > dTmp_0) REGS->CF = 1;
            else REGS->CF = 0;
            break;
        /*
            SUBRRL - Substract two registers (low byte)
                    and save result in first
            35 02 01 => ADRR R2, R1
        */
        case SUBRRL:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] SUBRRL" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            bTmp_2 = *(BYTE *) &REGS->R[bTmp_0];
            bTmp_1 = *(BYTE *) &REGS->R[bTmp_1];
            bTmp_1 -= bTmp_2;
            *(BYTE *) &REGS->R[bTmp_0] = bTmp_1;
            if(bTmp_1 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(bTmp_1 > bTmp_2) REGS->CF = 1;
            else REGS->CF = 0;
            break;
        /*
            XOR - Xor two registers
                and save result in first
            36 02 01 => XOR R2, R1
        */
        case XOR:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] XOR" << std::endl;
            #endif 
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            dTmp_0 = REGS->R[bTmp_0];
            dTmp_1 = REGS->R[bTmp_1];
            dTmp_2 = dTmp_0 ^ dTmp_1;
            if(dTmp_2 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            REGS->R[bTmp_0] = dTmp_2;
            REGS->CF = 0;
            break;
        /*
            XORL - Xor two registers (lower bytes)
                and save result in first
            37 02 01 => XOR R2, R1
        */
        case XORL:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] XORL" << std::endl;
            #endif 
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            bTmp_2 = REGS->R[bTmp_0];
            bTmp_1 = REGS->R[bTmp_1];
            bTmp_2 ^= bTmp_1;
            if(bTmp_2 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            *(BYTE *) &REGS->R[bTmp_0] = bTmp_2;
            REGS->CF = 0;
            break;
        /*
            NOT - Bitwise not on value in a register
                and save result in this register
            38 02 => NOT R2
        */
        case NOT:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] NOT" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            REGS->R[bTmp_0] = ~ REGS->R[bTmp_0];
            break;
        /*
            NOTB - Bitwise not on value in a register (lower bytes)
                and save result in this register
            39 02 => NOT R2
        */
        case NOTB:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] NOTB" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            *(BYTE *) &REGS->R[bTmp_0] = ~ (*(BYTE *) &REGS->R[bTmp_0]);
            break;
        /*
            ADVRD - Add double word value to register
            3A 02 10 00 00 00 => ADVR R2, 10
        */
        case ADVRD:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] ADVRD" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            dTmp_0 = *(DWORD*) &AS->codeData[REGS->PC];
            REGS->PC += 4;
            dTmp_1 = REGS->R[bTmp_0] + dTmp_0;          
            if(dTmp_1 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(dTmp_1 < REGS->R[bTmp_0]) REGS->CF = 1;
            else REGS->CF = 0;
            REGS->R[bTmp_0] = dTmp_1;
            break;
            /*
            SUBVRD - Substract double word value from register
            3B 02 10 00 00 00 => SUBVR R2, 10
        */
        case SUBVRD:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] SUBVRD" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            dTmp_0 = *(DWORD*) &AS->codeData[REGS->PC];
            REGS->PC += 4;
            dTmp_1 = REGS->R[bTmp_0] - dTmp_0;          
            if(wTmp_1 == 0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(wTmp_1 > REGS->R[bTmp_0]) REGS->CF = 1;
            else REGS->CF = 0;
            *(DWORD *) &REGS->R[bTmp_0] = dTmp_1;
            break;
        /*
            SHR - Shift the bits of the operand destination to the right,
                  by the number of bits specified in the count operand
            3C 02 05 => SHR R2, 5
        */
        case SHR:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] SHR" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            REGS->R[bTmp_0] = REGS->R[bTmp_0] >> bTmp_1;
            break;
        /*
            SHL -Shift the bits of the operand destination to the left,
                  by the number of bits specified in the count operand
            3D 02 05 => SHL R2, 5
        */
        case SHL:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] SHL" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            REGS->R[bTmp_0] = REGS->R[bTmp_0] << bTmp_1;
            break;
        /*  ********************************
                        COMPARE
            ********************************
        */
        /*
            CMP - compare two registers
            50 02 01 => CMP R2, R1
        */
        case CMP:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] CMP" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            dTmp_0 = REGS->R[bTmp_0];
            dTmp_1 = REGS->R[bTmp_1];
            #ifdef V_DEBUG
                std::cout << dTmp_0 << std::endl;
                std::cout << dTmp_1 << std::endl;
            #endif
            if(dTmp_1 == dTmp_0) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(dTmp_1 > dTmp_0) REGS->CF = 1;
            else REGS->CF = 0;
            break;
        /*
            CMPL - compare two registers (lower byte)
            51 02 01 => CMP R2, R1
        */
        case CMPL:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] CMPL" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            bTmp_1 = AS->codeData[REGS->PC++];
            if(bTmp_1 > 8) goto EXCEPTION;
            bTmp_0 = *(BYTE*) &REGS->R[bTmp_0];
            bTmp_1 = *(BYTE*) &REGS->R[bTmp_1];
            if(bTmp_0 == bTmp_1) REGS->ZF = 1;
            else REGS->ZF = 0;
            if(bTmp_0 < bTmp_1) REGS->CF = 1;
            else REGS->CF = 0;
            break;
        /*  ********************************
                        STACK
            ********************************
        */
        /*
            PUSH REGISTER
            90 03 => PUSH R3
        */
        case PUSH:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] PUSH" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            REGS->SP -= 1;
            if(REGS->SP == 0xFFFFFFFF) {
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::OVERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK OVERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            AS->stack[REGS->SP] = REGS->R[bTmp_0];
            #ifdef V_DEBUG
                std::cout <<"val: " << std::hex << AS->stack[REGS->SP] << std::endl;
            #endif
            break;
            /*
            POP TO A REGISTER
            91 03 => POP R3
        */
        case POP:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] POP" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 > 8) goto EXCEPTION;
            if((&AS->stack[REGS->SP]) == (&AS->stack[sizeof(AS->stack)/sizeof(DWORD)])){
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::UNDERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            REGS->R[bTmp_0] = AS->stack[REGS->SP];
            REGS->SP += 1;
            break;
        /*
            Clear the stack
            92 => CLST
        */
        case CLST:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] CLST" << std::endl;
            #endif
            memset(AS->stack, 0, STACK_SIZE*sizeof(*(AS->stack)));
            REGS->SP = 0;
            break;
        /*
            Set the stack pointer
            93 01 00 00 00 => SETSP 1
        */
        case SETSP:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] SETSP" << std::endl;
            #endif
            REGS->SP = 0;
            REGS->SP = *(DWORD *) &AS->codeData[REGS->PC];
            if(REGS->SP > STACK_SIZE) goto EXCEPTION;
            REGS->PC += 4;
            break;
        /*  ********************************
                        IN/OUT
            ********************************
        */
        /*
            POC - Print char without new line
                the value must be at the top of
                the stack
            A0 => POC
        */
        case POC:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] POC" << std::endl;
            #endif
            if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(DWORD)]){
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::UNDERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            bTmp_0 = *(BYTE*) &AS->codeData[AS->stack[REGS->SP++]];
            vmPrint(bTmp_0);
            break;
        /*
            POCN - Print char with new line
                the value must be at the top of
                the stack
            A1 => POCN
        */
        case POCN:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] POCN" << std::endl;
            #endif
            if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(DWORD)]){
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::UNDERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            bTmp_0 = *(BYTE*) &AS->codeData[AS->stack[REGS->SP++]];
            vmPrintN(bTmp_0);
            break;
        /*
            TIB - Take input and move to the data buffer,
                  the length of the string is stored in R[7]
            A2 => TIB
        */
        case TIB:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] TIB" << std::endl;
            #endif
            {
                memset(AS->dataBuffer, 0, INPUT_BUFFER_SIZE*sizeof(*(AS->dataBuffer)));
                std::string inData = "";
                std::getline(std::cin, inData);
                BYTE endOfText = 0x3;
                if(inData.length() > (INPUT_BUFFER_SIZE - 1)) 
                {
                    inData = inData.substr(0, (INPUT_BUFFER_SIZE - 1));
                }
                int counter = 0;
                for(char const &c: inData)
                {
                    AS->dataBuffer[counter++] = c;
                }
                AS->dataBuffer[counter++] = endOfText;
                REGS->R[7] = 0;
                *(WORD *) &REGS->R[7] = counter;
            }
            break;
        /*
            GIC - Get a specific char from input, that is stored in the data buffer,
                  the value will be stored in R[7],
                  pass the position of char via a some register
            A3 02 => GIC R2
        */
        case GIC:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] GIC" << std::endl;
            #endif
            bTmp_0 = AS->codeData[REGS->PC++];
            if(bTmp_0 >= 0 && bTmp_0 <= 7)
            {
                if(REGS->R[bTmp_0] >= INPUT_BUFFER_SIZE)
                {
                    goto EXCEPTION;
                }
                REGS->R[7] = 0;
                *(BYTE *) &REGS->R[7] = AS->dataBuffer[REGS->R[bTmp_0]];
            }
            else goto EXCEPTION;
            break;
        /*
            PIC - Print char from input without new line
                the value must be at the top of
                the stack
            A4 => PIC
        */
        case PIC:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] PIC" << std::endl;
            #endif
            if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(DWORD)]){
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::UNDERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            bTmp_0 = AS->stack[REGS->SP++];
            vmPrint(bTmp_0);
            break;
        /*
            PICN - Print char from input with new line
                the value must be at the top of
                the stack
            A5 => POCN
        */
        case PICN:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] PICN" << std::endl;
            #endif
            if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(DWORD)]){
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::UNDERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            bTmp_0 = AS->stack[REGS->SP++];
            vmPrintN(bTmp_0);
            break;
        /*
            PXV - Print a value in hex;
                the value must be at the top of the stack
            A6 => PXV
        */
        case PXV:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] PXV" << std::endl;
            #endif
            if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(DWORD)]){
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::UNDERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            dTmp_0 = AS->stack[REGS->SP++];
            vmPrintHX(dTmp_0);
            break;
        /*
            PXVN - Print a value in hex with a new line;
                the value must be at the top of the stack
            A7 => PXV
        */
        case PXVN:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] PXVN" << std::endl;
                // std::cout << std::bitset<32>(AS->stack[REGS->SP]) << std::endl;
            #endif
            if(&AS->stack[REGS->SP] == &AS->stack[sizeof(AS->stack)/sizeof(DWORD)]){
                #ifdef _VM_CPU_TEST_
                    vcpuFlag = VCpuFlag::UNDERFLOW;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] STACK UNDERFLOW!" << std::endl;
                #endif
                goto EXCEPTION;
            }
            dTmp_0 = AS->stack[REGS->SP++];
            vmPrintHXN(dTmp_0);
            break;
        /*  
            ********************************
                        DEFAULT
            ********************************
        */
        default:
            #ifdef V_DEBUG
                std::cout << "[DEBUG] DEFAULT" << std::endl;
            #endif
            EXCEPTION:
                #ifdef _VM_CPU_TEST_
                    if(vcpuFlag == VCpuFlag::OK) vcpuFlag = VCpuFlag::ERROR;
                #endif
                #ifndef _VM_CPU_TEST_
                    std::cout << "[ERROR] vCPU CRASH!" << std::endl;
                #endif
                valToReturn = 1;
    }

    return valToReturn;
}