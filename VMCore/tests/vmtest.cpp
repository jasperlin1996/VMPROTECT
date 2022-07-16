#include "vmtest.hpp"

std::ostream &operator << ( std::ostream& os, const VCpuFlag vcf ) {
    switch(vcf) {
        case VCpuFlag::OK:
            os << "OK";
        break;
        case VCpuFlag::ERROR:
            os << "ERROR";
        break;
        case VCpuFlag::UNDERFLOW:
            os << "UNDERFLOW";
        break;
        case VCpuFlag::OVERFLOW:
            os << "OVERFLOW";
        break;
    }
    return os;
}

void VMTest::printTestResult(bool bTestPass, unsigned int currentTestNumber)
{
    if(bTestPass == true) std::cout << "\t[INFO] TEST " << currentTestNumber << ": PASS" << std::endl;
    else std::cout << "\t[INFO] TEST " << currentTestNumber << ": FAIL!" << std::endl;
}

bool VMTest::testVM()
{
    bool bTestPass = true;
    unsigned int currentTestNumber = 0;

    T_AS = vm->getAS();
    T_REGS = vm->getREGS();

    /* ************************* */
        /* TEST 1 
        desc: check for unforeseen modifications
        */
    /* ************************* */
    ++currentTestNumber;
    T_AS->codeData[0] = NOP;
    T_AS->codeData[1] = EE;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_AS->codeData[T_REGS->PC] == 0x00) {
        for(int i = 0; i < 6; i++){
            if(T_REGS->R[i] != (VDWORD) 0) {
                bTestPass = false;
                goto FINISH_TESTS;
            }
        }
        for(int i = 3; i < 4096; i++){
            if(T_AS->codeData[i] != 0x00) {
                bTestPass = false;
                goto FINISH_TESTS;
            }
        }
    }
    else {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 2
        desc: check the correctness of data transfer from a register to a register
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[1] = (VDWORD) 5;
    T_REGS->R[2] = (VDWORD) 1;
    T_AS->codeData[0] = MOV;
    T_AS->codeData[1] = 0x01; // R1
    T_AS->codeData[2] = 0x02; // R2
    T_AS->codeData[3] = EE;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_REGS->R[2] == (VDWORD) 1) {
        if(T_REGS->R[1] != (VDWORD) 1) {
            bTestPass = false;
            goto FINISH_TESTS;
        }
    }
    else {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[1] = (VDWORD) 0;
    T_REGS->R[2] = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_AS->codeData[3] = 0x00;
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 3
        desc: check the correctness of data transfer (VBYTE and extend to VWORD) from memory to a register
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 6;
    T_AS->codeData[0] = MOVMB;
    T_AS->codeData[1] = 0x02; // R2
    T_AS->codeData[2] = 0x07;
    T_AS->codeData[3] = 0x00; // 0007h
    T_AS->codeData[4] = EE;
    T_AS->codeData[7] = 0x03;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_REGS->R[2] != (VDWORD) 3){
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_AS->codeData[3] = 0x00;
    T_AS->codeData[4] = 0x00;
    T_AS->codeData[7] = 0x00;
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 4
        desc: check the correctness of data transfer (VWORD) from memory to a register
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 0;
    T_REGS->R[2] = (VWORD) 6;
    T_AS->codeData[0] = MOVMW;
    T_AS->codeData[1] = 0x02; // R2
    T_AS->codeData[2] = 0x07;
    T_AS->codeData[3] = 0x00; // 0007h
    T_AS->codeData[4] = EE;
    T_AS->codeData[7] = 0x03;
    T_AS->codeData[8] = 0x04;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_REGS->R[2] != (VWORD) 0x0403){
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_AS->codeData[3] = 0x00;
    T_AS->codeData[4] = 0x00;
    T_AS->codeData[7] = 0x00;
    T_AS->codeData[8] = 0x00;
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 5
        desc: check the correctness of data transfer (VBYTE) from a register to memory
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 6;
    T_AS->codeData[0] = MOVBM;
    T_AS->codeData[1] = 0x07;
    T_AS->codeData[2] = 0x00; // 0007h
    T_AS->codeData[3] = 0x02; // R2
    T_AS->codeData[4] = EE;
    T_AS->codeData[7] = 0x03;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_REGS->R[2] == (VDWORD) 6) {
        if(T_AS->codeData[7] == 0x06) {
            if(T_AS->codeData[8] != 0x00) {
                bTestPass = false;
                goto FINISH_TESTS;
            }
        }
        else {
            bTestPass = false;
            goto FINISH_TESTS;
        }
    }
    else {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_AS->codeData[3] = 0x00;
    T_AS->codeData[4] = 0x00;
    T_AS->codeData[7] = 0x00;
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 6
        desc: check the correctness of data transfer (VWORD) from a register to memory
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 6;
    T_AS->codeData[0] = MOVWM;
    T_AS->codeData[1] = 0x07;
    T_AS->codeData[2] = 0x00; // 0007h
    T_AS->codeData[3] = 0x02; // R2
    T_AS->codeData[4] = EE;
    T_AS->codeData[7] = 0x03;
    T_AS->codeData[8] = 0x04;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_REGS->R[2] == (VDWORD) 6) {
        if(T_AS->codeData[7] == 0x06) {
            if(T_AS->codeData[8] != 0x00) {
                bTestPass = false;
                goto FINISH_TESTS;
            }
        }
        else {
            bTestPass = false;
            goto FINISH_TESTS;
        }
    }
    else {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_AS->codeData[3] = 0x00;
    T_AS->codeData[4] = 0x00;
    T_AS->codeData[7] = 0x00;
    T_AS->codeData[8] = 0x00;
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 7
        desc: check the stack underflow
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 6;
    T_AS->codeData[0] = POP;
    T_AS->codeData[1] = 0x02; // R2
    T_AS->codeData[2] = EE;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    else if(vm->vcpuFlag == VCpuFlag::OK){
        bTestPass = false;
        goto FINISH_TESTS;
    }
    else if(vm->vcpuFlag == VCpuFlag::UNDERFLOW){
        if(T_REGS->R[2] != (VDWORD) 6) {
            bTestPass = false;
            goto FINISH_TESTS;
        }
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 8
        desc: check the stack overflow
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 6;
    T_REGS->SP = 0;
    T_AS->codeData[0] = PUSH;
    T_AS->codeData[1] = 0x02; // R2
    T_AS->codeData[2] = EE;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    else if(vm->vcpuFlag == VCpuFlag::OVERFLOW){
        if(T_REGS->R[2] != (VDWORD) 6) {
            bTestPass = false;
            goto FINISH_TESTS;
        }
    }
    else {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_REGS->SP = sizeof(T_AS->stack) / sizeof(VDWORD);
    T_REGS->PC = (VDWORD) 0;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 9
        desc: shift the bits to the right
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 4;
    T_AS->codeData[0] = SHR;
    T_AS->codeData[1] = 0x02;
    T_AS->codeData[2] = 0x01;
    T_AS->codeData[3] = EE;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_REGS->R[2] != (VDWORD) 2){
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_REGS->PC = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_AS->codeData[3] = 0x00;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */
        /* TEST 10
        desc: shift the bits to the left
        */
    /* ************************* */
    ++currentTestNumber;
    T_REGS->R[2] = (VDWORD) 4;
    T_AS->codeData[0] = SHL;
    T_AS->codeData[1] = 0x02;
    T_AS->codeData[2] = 0x01;
    T_AS->codeData[3] = EE;
    vm->vcpuFlag = VCpuFlag::OK;
    vm->run();
    if(vm->vcpuFlag == VCpuFlag::ERROR) {
        bTestPass = false;
        goto FINISH_TESTS;
    }
    if(T_REGS->R[2] != (VDWORD) 8){
        bTestPass = false;
        goto FINISH_TESTS;
    }
    T_REGS->R[2] = (VDWORD) 0;
    T_REGS->PC = (VDWORD) 0;
    T_AS->codeData[0] = 0x00;
    T_AS->codeData[1] = 0x00;
    T_AS->codeData[2] = 0x00;
    T_AS->codeData[3] = 0x00;
    printTestResult(bTestPass, currentTestNumber);
    /* ************************* */

    goto FINISH_TESTS;

FINISH_TESTS:
    if(bTestPass == false) printTestResult(bTestPass, currentTestNumber);
    T_AS = NULL;
    T_REGS = NULL;
    return bTestPass;
}