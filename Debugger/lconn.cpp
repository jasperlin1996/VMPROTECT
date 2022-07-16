#include "main.hpp"
#ifdef _LINUX_DEV_ENVIRONMENT
int setLinuxConnection()
{
    int cliSocket = 0;
    struct sockaddr_in serv_addr;

    if ((cliSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        std::cout << "[ERROR] Failed socket creation\n"; 
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        std::cout << "[ERROR] Invalid address\n";
        return -1;
    }

    if (connect(cliSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        std::cout << "[ERROR] Connection failed\n";
        return -1;
    }

   return cliSocket;
}
#endif