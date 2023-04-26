#include "gui.h"

// #include <WinSock2.h>

void GUI::init() {
    WSADATA     wsadata;
    SOCKADDR_IN servADDR;
    SOCKADDR_IN servADDR_1;
    SOCKADDR_IN servADDR_2;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cout << "wsa start up failed!" << std::endl;
        exit(1);
    }

    struct hostent* myhost = gethostbyname("70s43w1165.imdo.co");
    char const*     hostIP = inet_ntoa(*(struct in_addr*)myhost->h_addr_list[0]);

    char const* hostPort = "17451";

    // const char* hostIP   = "192.168.1.100";
    // const char* hostPort = "6666";

    client.createSocket(servADDR, hostIP, hostPort);
    connect(client.clientSocket, (SOCKADDR*)&servADDR, sizeof(servADDR));

    client.createSocketRF(servADDR_1, hostIP, hostPort);
    connect(client.clientSocketRF, (SOCKADDR*)&servADDR_1, sizeof(servADDR_1));

    client.createSocketRM(servADDR_2, hostIP, hostPort);
    connect(client.clientSocketRM, (SOCKADDR*)&servADDR_2, sizeof(servADDR_2));

    std::cout << "connect successfully!" << std::endl;
}

void GUI::shutdown() {
    closesocket(client.clientSocket);
    closesocket(client.clientSocketRF);
    closesocket(client.clientSocketRM);
    WSACleanup();
}
