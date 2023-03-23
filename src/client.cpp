#include "client.h"

void Client::creatSocket(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort) {
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "creat socket failed!" << std::endl;
        exit(1);
    }

    memset(&hostAddr, 0, sizeof(hostAddr));
    hostAddr.sin_family = AF_INET;
    // hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_port        = htons(atoi(hostPort));
}

void Client::run() {
    logIn();
    while(1){}
}

bool Client::logIn() {
    auto pkg =
            PackageFactory::getInstance().createLoginPackage("cc12345678", "123456");
    // Sleep(5000);
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout<<"send successfully!"<<std::endl;
    recv(clientSocket, buffer, MAX_BUFFER, 0);
    Sleep(5000);
    // closesocket(clientSocket);
    return true;
}