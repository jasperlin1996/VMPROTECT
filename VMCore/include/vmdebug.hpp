#ifndef _VM_DEBUG
#define _VM_DEBUG

#include "./vmcpu.hpp"

#define LINUX_SOCKET
/*
Not support development on Windows right now!
#define WIN_SOCK
*/

#ifndef VMTESTS
    #ifdef LINUX_SOCKET

    #define PORT 9313
    #include "./nethelpers.hpp"

    #endif
#endif

#endif