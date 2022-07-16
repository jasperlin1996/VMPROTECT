#ifndef _VM_CPU_HPP
#define _VM_CPU_HPP

#include <iostream>
#include <string.h>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>
#include <utility>

#include "vmbase.hpp"
#include "screen.hpp"

#ifdef _WIN32_DEV_ENVIRONMENT
    #include <Windows.h>
#else _LINUX_DEV_ENVIRONMENT
    #include <unistd.h>
#endif

// #include <thread>
// #include <mutex>
// #include <condition_variable>

#include "../../SharedCode/global.hpp"
#include "../../SharedCode/opcodes.hpp"
#include "./advancesecurity.hpp"

#ifdef _WIN32_DEV_ENVIRONMENT
    #include "./drivers/win32sysbus.hpp"
#else //_LINUX_DEV_ENVIRONMENT
    #include "./drivers/linuxsysbus.hpp"
#endif

// #define V_DEBUG

#ifdef VMTESTS
    #include "./test.hpp"
#endif //VMTESTS

#include "./memory.hpp"
#include "../../SharedCode/datatypes.hpp"
#include "../../SharedCode/vmdebug.hpp"


#ifdef _VM_CPU_TEST_
    enum class VCpuFlag {OK = 0, ERROR, UNDERFLOW, OVERFLOW};
#endif // _VM_CPU_TEST_

class VMCPU: private VMBase {
    typedef void (VMCPU::*MFP)();

    public:
        bool areFramesNeeded;
        std::map<int, int> frameMap;

    private:
        PADDRESS_SPACE AS;
        PREGISTERSS REGS;
        #ifdef _WIN32_DEV_ENVIRONMENT
            WIN32 *sysBus;
        #else //_LINUX_DEV_ENVIRONMENT
            UNIX *sysBus;
        #endif

        // std::mutex memMutex;
        // std::condition_variable memConditionVar;
        int currentFrameNumber;
        bool isError;
        

    private:
        int executer(VBYTE);
        void getDataFromCodeData(std::string &, int);
        
        /* ******************** MEMORY *********************** */
        void writeByteIntoFrame(int, int, std::vector<VBYTE>);
        std::vector<VBYTE> getByteFromFrame(int, int);
        int loadFrame(int);
        void restoreFrame();
        /* ^^^^^^^^^^^^^ TODO: MOVE TO NEW CLASS ^^^^^^^^^^^^^ */

        int checkOpcodeSize(VBYTE, bool);
        void funcException(std::string e);

        // Opcodes functions
        void funcNop();
        void funcEE();
        void funcMov();
        void funcMovmb();
        void funcMovmw();
        void funcMovb();
        void funcMovw();
        void funcMovbm();
        void funcMovwm();
        void funcMovmrb();
        void funcMovmrw();
        void funcMovmd();
        void funcMovd();
        void funcMovdm();
        void funcMovmrd();
        void funcJmp();
        void funcJz();
        void funcJnz();
        void funcJae();
        void funcJbe();
        void funcJb();
        void funcJa();
        void funcAdvr();
        void funcAdrr();
        void funcAdrrl();
        void funcSubvr();
        void funcSubrr();
        void funcSubrrl();
        void funcXor();
        void funcXorl();
        void funcNot();
        void funcNotb();
        void funcAdvrd();
        void funcSubvrd();
        void funcShr();
        void funcShl();
        void funcCmp();
        void funcCmpl();
        void funcVmSysbus();
        void funcPush();
        void funcPop();
        void funcClSt();
        void funcSetSp();
        void funcPoc();
        void funcPocn();
        void funcTib();
        void funcGic();
        void funcPic();
        void funcPicn();
        void funcPxv();
        void funcPxvn();

        std::map <int, std::pair<int,MFP>> dOpcodesFunction = { 
            {0x56, {0,&VMCPU::funcNop}},
            {0x6d, {0,&VMCPU::funcNop}},
            {NOP, {0,&VMCPU::funcNop}},
            {EE, {0,&VMCPU::funcEE}},
            {MOV, {2,&VMCPU::funcMov}},
            {MOVMB, {3,&VMCPU::funcMovmb}},
            {MOVMW, {3,&VMCPU::funcMovmw}},
            {MOVB, {2,&VMCPU::funcMovb}},
            {MOVW, {3,&VMCPU::funcMovw}},
            {MOVBM, {3,&VMCPU::funcMovbm}},
            {MOVWM, {3,&VMCPU::funcMovwm}},
            {MOVMRB, {2,&VMCPU::funcMovmrb}},
            {MOVMRW, {2,&VMCPU::funcMovmrw}},
            {MOVMD, {3,&VMCPU::funcMovmd}},
            {MOVD, {5,&VMCPU::funcMovd}},
            {MOVDM, {3,&VMCPU::funcMovdm}},
            {MOVMRD, {2,&VMCPU::funcMovmrd}},
            {JMP, {2,&VMCPU::funcJmp}},
            {JZ, {2,&VMCPU::funcJz}},
            {JNZ, {2,&VMCPU::funcJnz}},
            {JAE, {2,&VMCPU::funcJae}},
            {JBE, {2,&VMCPU::funcJbe}},
            {JB, {2,&VMCPU::funcJb}},
            {JA, {2,&VMCPU::funcJa}},
            {ADVR, {3,&VMCPU::funcAdvr}},
            {ADRR, {2,&VMCPU::funcAdrr}},
            {ADRRL, {2,&VMCPU::funcAdrrl}},
            {SUBVR, {3,&VMCPU::funcSubvr}},
            {SUBRR, {2,&VMCPU::funcSubrr}},
            {SUBRRL, {2,&VMCPU::funcSubrrl}},
            {XOR, {2,&VMCPU::funcXor}},
            {XORL, {2,&VMCPU::funcXorl}},
            {NOT, {1,&VMCPU::funcNot}},
            {NOTB, {1,&VMCPU::funcNotb}},
            {ADVRD, {5,&VMCPU::funcAdvrd}},
            {SUBVRD, {5,&VMCPU::funcSubvrd}},
            {SHR, {2,&VMCPU::funcShr}},
            {SHL, {2,&VMCPU::funcShl}},
            {CMP, {2,&VMCPU::funcCmp}},
            {CMPL, {2,&VMCPU::funcCmpl}},
            {VMSYSBUS, {1,&VMCPU::funcVmSysbus}},
            {PUSH, {1,&VMCPU::funcPush}},
            {POP, {1,&VMCPU::funcPop}},
            {CLST, {0,&VMCPU::funcClSt}},
            {SETSP, {4,&VMCPU::funcSetSp}},
            {POC, {0,&VMCPU::funcPoc}},
            {POCN, {0,&VMCPU::funcPocn}},
            {TIB, {0,&VMCPU::funcTib}},
            {GIC, {1,&VMCPU::funcGic}},
            {PIC, {0,&VMCPU::funcPic}},
            {PICN, {0,&VMCPU::funcPicn}},
            {PXV, {0,&VMCPU::funcPxv}},
            {PXVN, {0,&VMCPU::funcPxvn}}
        };

    public:
        VMCPU();
        ~VMCPU();
        void run();
        void debug();
        bool loadCode(VBYTE *, int);
        bool loadCode(int, std::string);

    private:
        VBYTE* loadProtectedCode(int &, std::string);

    #ifdef _VM_CPU_TEST_
    public:
        static const bool bIsOnTest = true;
        VCpuFlag vcpuFlag;

        PADDRESS_SPACE getAS() { return AS; }
        PREGISTERSS getREGS() { return REGS; }
    #endif // _VM_CPU_TEST_
};

#endif //_VM_CPU_HPP