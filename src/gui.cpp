#include "gui.h"

// #include <WinSock2.h>

void GUI::init() {
    WSADATA     wsadata;
    SOCKADDR_IN servADDR;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cout << "wsa start up failed!" << std::endl;
        exit(1);
    }

    struct hostent* myhost = gethostbyname("70s43w1165.imdo.co");
    char const*     hostIP = inet_ntoa(*(struct in_addr*)myhost->h_addr_list[0]);

    char const* hostPort = "17451";

    client.createSocket(servADDR, hostIP, hostPort);
    connect(client.clientSocket, (SOCKADDR*)&servADDR, sizeof(servADDR));

    std::cout << "connect successfully!" << std::endl;
}

void GUI::shutdown() {
}
