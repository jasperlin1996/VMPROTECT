#ifndef _ADVM_SEC_HPP
#define _ADVM_SEC_HPP

#include <iostream>

#include "vmcpu.hpp"

/* ****** HEADERS ****** */
#ifdef _WIN32_DEV_ENVIRONMENT
    #include <intrin.h>
#else //_LINUX_DEV_ENVIRONMENT
    #include <unistd.h>
    #include <sys/ptrace.h>
    #include <cpuid.h>
#endif

class AdvanceSecurity {
    public:
        #ifdef _WIN32_DEV_ENVIRONMENT
            /* ****** TBD ****** */
        #else //_LINUX_DEV_ENVIRONMENT
            static void checkPtrace(int);
        #endif
        static void isHypervisor(void);
};

#endif //_ADVM_SEC_HPP