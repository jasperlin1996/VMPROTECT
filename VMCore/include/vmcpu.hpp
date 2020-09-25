#ifndef _VM_CPU_HPP
#define _VM_CPU_HPP

#include <iostream>
#include <string.h>
#include <stdint.h>

#include "./opcodes.hpp"

#ifdef VMTESTS
    #include "./test.hpp"
#endif //VMTESTS

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#define CODE_DATA_SIZE 51200
#define STACK_SIZE 256
#define INPUT_BUFFER_SIZE 1024

#include "./vmdebug.hpp"

typedef struct {
    /* Here will be a code to execute and other data - 50KB*/
    BYTE codeData[CODE_DATA_SIZE];

    /* Size of one element is DWORD 
    in order to be able to push addresses. */
    DWORD stack[STACK_SIZE];

    /* Here will be a user input*/
    BYTE dataBuffer[INPUT_BUFFER_SIZE];
} ADDRESS_SPACE, *PADDRESS_SPACE;

typedef struct {
    /* General Purpose Registers R0 -> R7 */
    DWORD R[8];
    struct {
        /* Zero Flag 
            value 1 - flag is set if the result of the last comparison was zero
            value 0 - flag is not set
        */
        unsigned char ZF : 1;
        /* Carry Flag 
            value 1 - flag is set the results of the last comparison was moving
            value 0 - flag is not set
        */
        unsigned char CF : 1;
    };
    /* Program Counter */
    DWORD PC;
    /* Stack Pointer */
    DWORD SP;
} REGISTERSS, *PREGISTERSS;


#ifdef _VM_CPU_TEST_
    enum class VCpuFlag {OK = 0, ERROR, UNDERFLOW, OVERFLOW};
#endif // _VM_CPU_TEST_

class VMCPU {
    private:
        PADDRESS_SPACE AS;
        PREGISTERSS REGS;

    private:
        int executer(BYTE);
        void vmPrint(BYTE s);
        void vmPrintHX(DWORD);
        void vmPrintN(BYTE s);
        void vmPrintHXN(DWORD);
        //void vmScan();

    public:
        VMCPU();
        ~VMCPU();
        void run();
        void debug();
        bool loadCode(BYTE *, int);

    #ifdef _VM_CPU_TEST_
    public:
        static const bool bIsOnTest = true;
        VCpuFlag vcpuFlag;

        PADDRESS_SPACE getAS() { return AS; }
        PREGISTERSS getREGS() { return REGS; }
    #endif // _VM_CPU_TEST_
};

#endif //_VM_CPU_HPP