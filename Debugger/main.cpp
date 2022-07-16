#include "main.hpp"

int main()
{
    printIntro();
    int cliSocket;
#ifdef _WIN32_DEV_ENVIRONMENT
    cliSocket = setWin32Connection();
#else _LINUX_DEV_ENVIRONMENT
    cliSocket = setLinuxConnection();
#endif
    if(cliSocket == -1) return -1;
    else conn(cliSocket);
    return 0;
}