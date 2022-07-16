#include "../include/vmcpu.hpp"

#ifdef _WIN32_DEV_ENVIRONMENT
    #include <string>
#endif

// #include <bitset>

VMCPU::VMCPU()
{
    AS = (PADDRESS_SPACE) new ADDRESS_SPACE;
    REGS = (PREGISTERSS) new REGISTERSS;

    memset(AS->codeData, 0, sizeof(AS->codeData));

    REGS->PC = 0;
    REGS->SP = sizeof(AS->stack) / sizeof(VDWORD);

    areFramesNeeded = false;
    currentFrameNumber = 0;
    isError = false;

    #ifdef _WIN32_DEV_ENVIRONMENT
        sysBus = new WIN32();
    #else //_LINUX_DEV_ENVIRONMENT
        sysBus = new UNIX();
    #endif
}

VMCPU::~VMCPU()
{
    if(areFramesNeeded)
    {
        std::string fileNameToRemove;
        for (const auto& [key, value] : frameMap) 
        {
            fileNameToRemove = "./.cached." + std::to_string(key) + ".frame";
            int retVal = sysBus->deleteFile(fileNameToRemove);
        }
    }
    delete AS;
    delete REGS;
    delete sysBus;
    dOpcodesFunction.clear();
}

void VMCPU::run()
{
    isError = false;
    bool exit = false;
    VBYTE opcode;

    while(!exit)
    {
        if(areFramesNeeded && (REGS->PC >= frameMap[currentFrameNumber])) REGS->PC = loadFrame(REGS->PC);
        if(isError) return;
        opcode = AS->codeData[REGS->PC++];
        exit = executer(opcode);
    }
    return;
}
