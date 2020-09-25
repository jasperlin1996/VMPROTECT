/*
    VMCore - a virtual machine that simulates a CPU, allows to perform arithmetic operations, 
    reads and writes to memory and interacts with I/O devices. It can understand a machine
    language which can be used to program it.
    
    Copyright (C) eaglx.
    version 0.2.200924.1423
*/

#include "../include/main.hpp"
#include "../include/vmcpu.hpp"
#include "../include/protected.hpp"
// #include <sstream>

void show_usage()
{
    std::cout << "Usage:"
                << "\t-m value\t\tSet program mode. The value can be debug or exec\n"
                << "\t-p filename\t\tPath to a file to execute"
                << std::endl;
}

int main(int argc, char *argv[])
{
    VMCPU *vm = new VMCPU();

    if(argc == 1) {
        if(ProtectedData[0] == 0xFF)
        {
            show_usage();
            return 0;
        }
        else
        {
            if(!vm->loadCode(ProtectedData, sizeof(ProtectedData)/sizeof(ProtectedData[0])))
            {
                return -1;
            }
            
            try{
                vm->run();
                return 0;
            } catch(...){
                return -1;
            }
        }
    }

    std::string mode = "";
    std::string pathToFile = "";

    for(int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if(arg == "-h")
        {
            show_usage();
            return 0;
        }
        else if(arg == "-m")
        {
            i += 1;
            mode =  argv[i];
        }
        else if(arg == "-p")
        {
            i += 1;
            pathToFile = argv[i];
        }
        else 
        {
            show_usage();
            return 0;
        }
        
    }

    BYTE *mc;
    int mcsize = -1;
    try
    {
        mc = loadProtectedCode(mcsize, pathToFile);
    }
    catch (int e)
    {
        std::cout << "[ERROR " << e << "] NO FILE OR SE \n";
        return -1;
    }

    if(!vm->loadCode(mc, mcsize))
    {
        delete[] mc;
        return -1;
    }

    delete[] mc;

    if(mode.compare(MODE_DEBUG) == 0)
    {
        vm->debug();
    }
    else if(mode.compare(MODE_EXEC) == 0)
    {
        try{
            vm->run();
        } catch(...){
            return -1;
        }
    }
    else
    {
        std::cout << "[ERROR 100101] INCORRECT MODE!\n";
        return -1;
    }

    return 0;
}
