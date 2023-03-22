#include <iostream>

#include "client.h"

int main() {
#ifdef WINDOWS
    std::cout << "Windows";
#elif LINUX
    std::cout << "Linux";
#endif
    endl(std::cout);
    WSADATA     wsadata;
    SOCKET      windowsSocket;
    SOCKADDR_IN servADDR;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cout << "wsa start up failed!" << std::endl;
        exit(1);
    }

    Client client;
    char const*  hostIP   = "192.168.1.103"; // TODO:这一部分后面需要更改，可能需要改成NAT穿透
    char const*  hostPort = "6666";

    windowsSocket = client.creatSocket(servADDR, hostIP, hostPort);
    // 到这一行就已经完成了套接字的创建，接下来就是链接目标主机进行登录的部分了。
    connect(windowsSocket, (SOCKADDR*)&servADDR, sizeof(servADDR));
    std::cout<<"successful!"<<std::endl;
    Sleep(20000);
    return 0;
}