#include "gui.h"

// #include <WinSock2.h>

void GUI::init() {
    threadPool = std::make_unique<Chen::ThreadPool>(4);

    WSADATA     wsadata;
    SOCKADDR_IN servADDR;
    SOCKADDR_IN servADDR_1;
    SOCKADDR_IN servADDR_2;
    SOCKADDR_IN servADDR_3;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cout << "wsa start up failed!" << std::endl;
        exit(1);
    }

    // struct hostent* myhost = gethostbyname("70s43w1165.imdo.co");
    // char const*     hostIP = inet_ntoa(*(struct in_addr*)myhost->h_addr_list[0]);

    // char const* hostPort = "17451";

    // const char* hostIP   = "192.168.1.100";
    // const char* hostPort = "6666";

    const char* hostIP   = "81.70.43.103";
    const char* hostPort = "8080";

    client.createSocket(servADDR, hostIP, hostPort);
    connect(client.clientSocket, (SOCKADDR*)&servADDR, sizeof(servADDR));

    client.createSocketRF(servADDR_1, hostIP, hostPort);
    connect(client.clientSocketRF, (SOCKADDR*)&servADDR_1, sizeof(servADDR_1));

    client.createSocketRM(servADDR_2, hostIP, hostPort);
    connect(client.clientSocketRM, (SOCKADDR*)&servADDR_2, sizeof(servADDR_2));

    client.createSocketSF(servADDR_3, hostIP, hostPort);
    connect(client.clientSocketSF, (SOCKADDR*)&servADDR_3, sizeof(servADDR_3));

    std::cout << "connect successfully!" << std::endl;
}

void GUI::shutdown() {
    closesocket(client.clientSocket);
    closesocket(client.clientSocketRF);
    closesocket(client.clientSocketRM);
    closesocket(client.clientSocketSF);

    WSACleanup();
}
