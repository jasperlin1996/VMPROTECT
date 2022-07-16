#ifndef _VM_TEST_HPP
#define _VM_TEST_HPP

#define _VM_CPU_TEST_

#include "../include/vmcpu.hpp"

class VMTest {
    private:
        VMCPU *vm;
        PADDRESS_SPACE T_AS;
        PREGISTERSS T_REGS;
        void printTestResult(bool, unsigned int);

    public:
        VMTest() {
            vm = new VMCPU();
        }
        ~VMTest() {
            delete vm;
        }
        bool testVM();
};
#endif