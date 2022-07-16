/*
    VMCore - a virtual machine that simulates a CPU, allows to perform arithmetic operations, 
    reads and writes to memory and interacts with I/O devices. It can understand a machine
    language which can be used to program it.
    
    Copyright (C) eaglx.
    version 0.4.190222.2030
*/
#include "../../SharedCode/global.hpp"
#include "../include/main.hpp"
#include "../include/vmcpu.hpp"
#include "../include/protected.hpp"
// #include <sstream>

void showUsage()
{
    std::cout << "Usage:"
                << "\t-m value\t\tSet program mode. The value can be debug or exec\n"
                << "\t-p filename\t\tPath to a file to execute"
                << std::endl;
}

int runVM(int argc, char *argv[])
{
    VMCPU *vm = new VMCPU();

    if(argc == 1) {
        if(ProtectedData[0] == 0xFF)
        {
            showUsage();
            return 0;
        }
        else
        {
            if(!vm->loadCode(ProtectedData, sizeof(ProtectedData)/sizeof(ProtectedData[0]))) return -1; //TODO: rewrite
  
            try {
                vm->run();
                return 0;
            } catch(...) {
                return -1;
            }
        }
    }

    std::string mode = "";
    std::string path_to_file = "";

    for(int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if(arg == "-h") {
            showUsage();
            return 0;
        }
        else if(arg == "-m") {
            i += 1;
            mode =  argv[i];
        }
        else if(arg == "-p") {
            i += 1;
            path_to_file = argv[i];
        }
        else {
            showUsage();
            return 0;
        }
    }

    try { if(!vm->loadCode(0, path_to_file)) { delete vm; return -1; } }
    catch (int e) {
        std::cout << "[ERROR " << e << "] NO FILE OR SE \n";
        return -1;
    }

    if(mode.compare(MODE_DEBUG) == 0) vm->debug();
    else if(mode.compare(MODE_EXEC) == 0) {
        try{ vm->run(); delete vm; } catch(...) { return -1; }
    }
    else {
        std::cout << "[ERROR 100101] INCORRECT MODE!\n";
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    #ifdef _WIN32_DEV_ENVIRONMENT
        AdvanceSecurity::isHypervisor();
        return runVM(argc, argv);
    #else //_LINUX_DEV_ENVIRONMENT
        AdvanceSecurity::isHypervisor();
        AdvanceSecurity::checkPtrace(argc);
        return runVM(argc, argv);
    #endif
}
