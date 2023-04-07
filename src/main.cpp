#pragma warning(disable : 4996)

#include <WinSock2.h>
#include <iostream>
#include <string>

#include "client.h"
#include "gui.h"
#include "package_factory.h"
#include "parser.h"

int main() {
    WSADATA     wsadata;
    SOCKADDR_IN servADDR;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cout << "wsa start up failed!" << std::endl;
        exit(1);
    }

    Client client;

    // struct hostent* myhost = gethostbyname("70s43w1165.imdo.co");
    // char const*     hostIP = inet_ntoa(*(struct in_addr*)myhost->h_addr_list[0]);

    // TODO:这一部分后面需要更改，可能需要改成NAT穿透
    char const* hostIP   = "172.20.144.1";
    char const* hostPort = "6666";
    // char const* hostPort = "17451";

    client.createSocket(servADDR, hostIP, hostPort);
    connect(client.clientSocket, (SOCKADDR*)&servADDR, sizeof(servADDR));

    std::cout << "connect successfully!" << std::endl;

    client.run();

    return 0;
}
