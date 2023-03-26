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
        std::cout<<"what's your option? "<<std::endl<<"-1 sendMsg2User"<<std::endl<<"-2 receiveMsgFromUser"<<std::endl;
        std::cout<<"-3 senMsg2Group"<<std::endl<<"-4 sendFile2User"<<std::endl;
        std::cout<<"-5 recvFileFromUser"<<std::endl<<"-4 sendFile2User"<<std::endl;


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
            readMsg();
        }

        if(op == 3){ // 发送组消息
            std::string groupTarget;
            std::string msg;
            
            std::cout<<"what's your target?"<<std::endl;
            std::cin>>groupTarget;
            std::cout<<"what's your msg?"<<std::endl;
            std::cin>>msg;
            
            // std::cout<<"here 1"<<std::endl;
            Msg2Group(groupTarget, msg);
        }

        if(op == 4){ // 传送文件
            std::string filename;
            std::string target;

            std::cout<<"who do you want to transfer a file?"<<std::endl;
            std::cin>> target;
            std::cout<<"what file do you want to transfer?"<<std::endl;
            std::cin>>filename;

            transferFile(target, filename);
        }
        
        if(op == 5){ // 接收文件
            recvFile();
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
    // std::cout<<"here 2"<<std::endl;
    auto pkg =
            PackageFactory::getInstance().createPackage2(myName, target, msg);
    std::cout<<"pkg2 created successfully!"<<std::endl;
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout<<"send user successfully!"<<std::endl;
    // recv(clientSocket, buffer, MAX_BUFFER, 0);
}

void Client::readMsg(){
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

void Client::Msg2Group(std::string groupTarget, std::string msg){
    auto pkg =
        PackageFactory::getInstance().createPackage3(myName, groupTarget, msg);
    std::cout<<"pkg3 created successfully!"<<std::endl;
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout<<"send group msg successfully!"<<std::endl;
}



void Client::transferFile(std::string target, std::string filename){
    FILE* fp = fopen(filename.c_str(), "r");
    // TODO: 文件路径名拼接， 文件放在../file/
    

    // 开始传文件
    int msgIndex = 0;
    while ((true)){
        int read_length = fread(buffer, 1, MAX_BUFFER - 40, fp);

        std::string tem;
        if(read_length != 0){
            tem.resize(read_length);
            memcpy(const_cast<char*>(tem.c_str()), buffer, read_length);
        }

        memset(buffer, 0, MAX_BUFFER);

        auto pkg =
            PackageFactory::getInstance().createPackage4(myName, target, msgIndex, filename, tem);
        if(read_length < MAX_BUFFER - 40){
            send(clientSocket, pkg.start, pkg.size, 0);
            break;
        }
        send(clientSocket, pkg.start, pkg.size, 0);

        msgIndex += read_length;
    }
    std::cout<<"文件传输完成"<<std::endl;
}


void Client::recvFile(){
    memset(buffer, 0, MAX_BUFFER);
    recv(clientSocket, buffer, MAX_BUFFER, 0);
    Parser parser;

    parser.parsePkgHead(buffer);
    parser.parseMsg(buffer);

    std::string filename = parser.info.filename;

    std::map<std::string, int>::iterator iter = fileIndex.find(filename);
    if(iter == fileIndex.end()){
        fileIndex.insert(std::pair<std::string, int>(filename, 0));
    }

    if(int(parser.info.opcode) == 4){ // 非断传报文
        // TODO: 路径名放到别的文件夹下可能要更改
        FILE* fp = fopen(filename.c_str(), "w"); // 非接收重传文件为w
        
        int recv_count;
        while( (recv_count = recv(clientSocket, buffer, MAX_BUFFER, 0)) != 0){
            fwrite((void*)buffer, 1, recv_count, fp);
            fileIndex[filename] += recv_count;
        }

        std::cout<<"文件接收完成"<<std::endl;
    }

    if(int(parser.info.opcode) == 6){ // 断传报文
        // TODO: 路径名放到别的文件夹下可能要更改
        FILE* fp = fopen(filename.c_str(), "a"); // 非接收重传文件为w
        
        int recv_count;
        while( (recv_count = recv(clientSocket, buffer, MAX_BUFFER, 0)) != 0){
            fwrite((void*)buffer, 1, recv_count, fp);
            fileIndex[filename] += recv_count;

        }
        std::cout<<"文件断传完成"<<std::endl;
    }
    
}












