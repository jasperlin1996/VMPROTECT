#include "main.hpp"

void printIntro()
{
    std::cout << R"(
 _   ____  _______________ _____ _____ _____ _____ _____ 
| | | |  \/  || ___ \ ___ \  _  |_   _|  ___/  __ \_   _|
| | | | .  . || |_/ / |_/ / | | | | | | |__ | /  \/ | |  
| | | | |\/| ||  __/|    /| | | | | | |  __|| |     | |  
\ \_/ / |  | || |   | |\ \\ \_/ / | | | |___| \__/\ | |  
 \___/\_|  |_/\_|   \_| \_|\___/  \_/ \____/ \____/ \_/  
                                                         
                                                         
______ ___________ _   _ _____ _____  ___________        
|  _  \  ___| ___ \ | | |  __ \  __ \|  ___| ___ \       
| | | | |__ | |_/ / | | | |  \/ |  \/| |__ | |_/ /       
| | | |  __|| ___ \ | | | | __| | __ |  __||    /        
| |/ /| |___| |_/ / |_| | |_\ \ |_\ \| |___| |\ \        
|___/ \____/\____/ \___/ \____/\____/\____/\_| \_|       
                                                         
    )" << std::endl;
    std::cout << "version 0.3.220711.2120\n" << std::endl;
}

void printOptions()
{
    std::cout << "Options:\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::EXEC) << ". Execute a program on the VM\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::STEP) << ". Step execution\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::EXIT_DBG) << ". Exit debugger and exit debug mode in the VM\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::SET_VAL) << ". Set a value in a register\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::SET_FLAG) << ". Set a flag\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::SHOW_STACK) << ". Show the stack\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::SHOW_CODE_DATA) << ". Show the code data\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::SHOW_DATA_BUFFER) << ". Show the data buffer\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::SHOW_REGS) << ". Show all registers\n"
                << "\t" << static_cast<int>(DEBUG_OPTIONS::WRITE_CODE) << ". Write to the code data\n"
                << std::endl;
    std::cout << "Choice: ";
}

void printRegToModifyOptions()
{
    std::cout << "Select a register to modify:\n"
                << "\t1. PC\n"
                << "\t2. SP\n"
                << "\t3. Rx\n";
    std::cout << "Choice: ";
}

void printFlagToModifyOptions()
{
    std::cout << "Select a flag to modify:\n"
                << "\t1. ZF\n"
                << "\t2. CF\n";
    std::cout << "Choice: ";
}


void errorSend(int cliSocket)
{
    std::cout << "Failed send data in debug\n";
#ifdef  _LINUX_DEV_ENVIRONMENT
    close(cliSocket);
#else _WIN32_DEV_ENVIRONMENT
    closesocket(cliSocket);
    WSACleanup();
#endif
    exit(-1);
}