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
    if(!logIn()){
        closesocket(clientSocket);
        return;
    }
    while(1){}
}

bool Client::logIn() {
    auto pkg =
            PackageFactory::getInstance().createLoginPackage("cc12345678", "1234561");
    // Sleep(5000);
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout<<"send successfully!"<<std::endl;
    recv(clientSocket, buffer, MAX_BUFFER, 0);
    Parser parser;
    parser.parsePkgHead(buffer);
    std::cout<<int(parser.info.opcode)<<std::endl;
    parser.parseMsg(buffer);

    char type = char(* parser.msg.c_str());

    std::cout<<"-----"<<parser.msg<<std::endl;
    if(type == 'a'){
        std::cout<<"log in successfully!"<<std::endl;
        return true;
    }
    else if (type == 'b'){
        std::cout<<"your pwd is wrong!"<<std::endl;
        return false;
    }
    else if(type == 'c'){
        std::cout<<"this account does not exist!"<<std::endl;
        return false;
    }
    Sleep(5000);
    // closesocket(clientSocket);
    return false;
}