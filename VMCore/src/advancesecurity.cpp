#include "../include/advancesecurity.hpp"

#ifdef _WIN32_DEV_ENVIRONMENT
    /* ****** TBD ****** */
#else //_LINUX_DEV_ENVIRONMENT
    void AdvanceSecurity::checkPtrace(int argc) {
        int offset = 10;
        if (ptrace(PTRACE_TRACEME, 0, 1, 0) == 0) offset = 66;
        if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) offset *= 23;
        if (offset != 66 * 23) {
            unsigned milliseconds = 100 * argc;
            VMCPU *vm = new VMCPU();
            usleep(milliseconds * 1000);
            exit(0);
        }
    }
#endif

void AdvanceSecurity::isHypervisor(void) {
    #ifdef _WIN32_DEV_ENVIRONMENT
        int cpuinfo[4]; 
        __cpuid(cpuinfo, 1);
        if (cpuinfo[2] >> 31 & 1) exit(0);
    #else
        unsigned int eax, ebx, ecx, edx;
        __get_cpuid (1, &eax, &ebx, &ecx, &edx);
        if (ecx >> 31 & 1) exit(0);
    #endif
    
    return;
}