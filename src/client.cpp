#include "client.h"

void Client::creatSocket(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort) {
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "creat socket failed!" << std::endl;
        exit(1);
    }

    memset(&hostAddr, 0, sizeof(hostAddr));
    hostAddr.sin_family      = AF_INET;
    // hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_port        = htons(atoi(hostPort));

}

void run(){
    
}

bool logIn(){
    char account[10];
    char pwd[10];
    std::cout<<"请输入账号：";
    std::cin>>account;\
    std::cout<<"请输入密码：";
    std::cin>>pwd;
    
}