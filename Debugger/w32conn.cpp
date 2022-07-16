#include "main.hpp"
#ifdef _WIN32_DEV_ENVIRONMENT
int setWin32Connection()
{
    //Initialize Winsock
    WSADATA wsaData;
    int status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (status != NO_ERROR)
    {
        std::cout << "[ERROR " << status << "] WSAStartup Failed"<< std::endl;
        return -1;
    }

    SOCKET cliSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (cliSocket == INVALID_SOCKET)
    {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &addrServer.sin_addr.s_addr);
    addrServer.sin_port = htons(PORT);
    memset(&(addrServer.sin_zero), '\0', 8);

    // Connect to server.
    status = connect(cliSocket, (SOCKADDR*)&addrServer, sizeof(addrServer));
    if (status == SOCKET_ERROR)
    {
        closesocket(cliSocket);
        std::cout << "[ERROR] Unable to connect VMCore" << std::endl;
        WSACleanup();
        return -1;
    }

    return cliSocket;
}
#endif