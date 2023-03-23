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
    if(logIn("cc12345678", "123456") != 1){
        closesocket(clientSocket);
        return;
    }
    while(1){
        int op;
        std::cout<<"what's your option? "<<std::endl<<"-1 sendMsg2User"<<std::endl<<"-2 receiveMsg"<<std::endl;
        std::cin>>op;
        
        if(op == 1){ // 1代表给目标单发消息
            std::string target;
            std::string msg;
            
            std::cout<<"what's your target?"<<std::endl;
            std::cin>>target;
            std::cout<<"what's your msg?"<<std::endl;
            std::cin>>msg;
            
            std::cout<<"here 1"<<std::endl;
            Msg2User(target, msg);
        }

        Sleep(100);

        if(op == 2){
            readMsgFromUser();
        }
        
    }
}

int Client::logIn(std::string account, std::string pwd) {
    
    std::cout<<"put in your account"<<std::endl;
    std::cin>>myName;
    std::cout<<"put in your pwd"<<std::endl;
    std::cin>>myPwd;
    
    
    
    // myName = account;
    // myPwd = pwd;
    
    auto pkg =
            PackageFactory::getInstance().createLoginPackage(myName.c_str(), myPwd);
    // Sleep(5000);
    send(clientSocket, pkg.start, pkg.size, 0);
    // std::cout<<"send successfully!"<<std::endl;
    recv(clientSocket, buffer, MAX_BUFFER, 0);
    
    
    Parser parser;
    parser.parsePkgHead(buffer);
    // std::cout<<int(parser.info.opcode)<<std::endl;
    parser.parseMsg(buffer);
    memset(buffer, 0, MAX_BUFFER);


    char type = char(* parser.msg.c_str());

    // std::cout<<"-----"<<parser.msg<<std::endl;
    if(type == 'a'){
        std::cout<<"log in successfully!"<<std::endl;
        return 1;
    }
    else if (type == 'b'){
        std::cout<<"your pwd is wrong!"<<std::endl;
        return 2;
    }
    else if(type == 'c'){
        std::cout<<"this account does not exist!"<<std::endl;
        return 3;
    }
    Sleep(5000);
    // closesocket(clientSocket);
    return 0;
}

void Client::Msg2User(std::string target, std::string msg){
    std::cout<<"here 2"<<std::endl;
    auto pkg =
            PackageFactory::getInstance().createPackage2(myName, target, msg);
    std::cout<<"pkg created successfully!"<<std::endl;
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout<<"send successfully!"<<std::endl;
    // recv(clientSocket, buffer, MAX_BUFFER, 0);
}

void Client::readMsgFromUser(){
    memset(buffer, 0, MAX_BUFFER);
    recv(clientSocket, buffer, MAX_BUFFER, 0);
    Parser parser;

    parser.parsePkgHead(buffer);
    parser.parseMsg(buffer);

    std::cout<<"account"<<parser.info.account<<std::endl;
    std::cout<<"target"<<parser.info.target<<std::endl;
    std::cout<<"msglen"<<parser.info.msglen<<std::endl;
    
    std::cout<<"msg"<<parser.msg<<std::endl;
    Sleep(50000);
}






