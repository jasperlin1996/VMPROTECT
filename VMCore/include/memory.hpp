#ifndef _MEMORY_VM_HPP
#define _MEMORY_VM_HPP

#include "../../SharedCode/datatypes.hpp"
#include "../../SharedCode/memsize.hpp"

typedef struct {
    /* Here will be a code to execute and other data - 50KB*/
    VBYTE codeData[CODE_DATA_SIZE];

    /* Size of one element is VDWORD 
    in order to be able to push addresses. */
    VDWORD stack[STACK_SIZE];

    /* Here will be a user input*/
    VBYTE dataBuffer[INPUT_BUFFER_SIZE];
} ADDRESS_SPACE, *PADDRESS_SPACE;

typedef struct {
    /* General Purpose Registers R0 -> R7 */
    VDWORD R[8];
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
    VDWORD PC;
    /* Stack Pointer */
    VDWORD SP;
} REGISTERSS, *PREGISTERSS;


class Memory {  /* ************* TBD ************* */
    private:
        // void writeByteIntoFrame(int, int, std::vector<VBYTE>);
        // std::vector<VBYTE> getByteFromFrame(int, int);
        // int loadFrame(int);
        // void restoreFrame();
        // PADDRESS_SPACE AS;
        // PREGISTERSS REGS;
        
        // std::map<int, int> frameMap;

    public:
        // bool areFramesNeeded;
        Memory() {}
};

#endif //_MEMORY_VM_HPP