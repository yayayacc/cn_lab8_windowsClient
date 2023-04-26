#include "client.h"

#include <filesystem>

#include "gui.h"

void Client::createSocket(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort) {
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

void Client::createSocketRM(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort) {
    clientSocketRM = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocketRM == INVALID_SOCKET) {
        std::cout << "creat socket failed!" << std::endl;
        exit(1);
    }

    memset(&hostAddr, 0, sizeof(hostAddr));
    hostAddr.sin_family = AF_INET;
    // hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_port        = htons(atoi(hostPort));
}

void Client::createSocketRF(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort) {
    clientSocketRF = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocketRF == INVALID_SOCKET) {
        std::cout << "creat socket failed!" << std::endl;
        exit(1);
    }

    memset(&hostAddr, 0, sizeof(hostAddr));
    hostAddr.sin_family = AF_INET;
    // hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_addr.s_addr = inet_addr(hostIP);
    hostAddr.sin_port        = htons(atoi(hostPort));
}

void Client::createSocketSF(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort) {
    clientSocketSF = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocketSF == INVALID_SOCKET) {
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
    if (logIn("cc12345678", "123456") != 1) {
        closesocket(clientSocket);
        closesocket(clientSocketRF);
        closesocket(clientSocketRM);
        return;
    }
    while (1) {
        int op;
        std::cout << "what's your option? " << std::endl
                  << "-1 sendMsg2User" << std::endl
                  << "-2 receiveMsgFromUser" << std::endl;
        std::cout << "-3 senMsg2Group" << std::endl
                  << "-4 sendFile2User" << std::endl;
        std::cout << "-5 recvFileFromUser" << std::endl
                  << "-6 askReTransfer" << std::endl;
        std::cout << "-7 reTransfer" << std::endl
                  << "-8 close Client" << std::endl;

        std::cin >> op;

        if (op == 1) { // 1代表给目标单发消息
            std::string target;
            std::string msg;

            std::cout << "what's your target?" << std::endl;
            std::cin >> target;
            std::cout << "what's your msg?" << std::endl;
            std::cin >> msg;

            std::cout << "here 1" << std::endl;
            Msg2User(target, msg);
        }

        Sleep(100);

        if (op == 2) {
            readMsg();
        }

        if (op == 3) { // 发送组消息
            std::string groupTarget;
            std::string msg;

            std::cout << "what's your target?" << std::endl;
            std::cin >> groupTarget;
            std::cout << "what's your msg?" << std::endl;
            std::cin >> msg;

            // std::cout<<"here 1"<<std::endl;
            Msg2Group(groupTarget, msg);
        }

        if (op == 4) { // 传送文件
            std::string filename;
            std::string target;

            std::cout << "who do you want to transfer a file?" << std::endl;
            std::cin >> target;
            std::cout << "what file do you want to transfer?" << std::endl;
            std::cin >> filename;

            transferFile(target, filename);
        }

        if (op == 5) { // 接收文件
            recvFile();
        }

        if (op == 6) { // 请求断传某个文件
            std::string filename;
            std::string target;

            std::cout << "who do you want to receive a file from?" << std::endl;
            std::cin >> target;
            std::cout << "what file do you want to receive?" << std::endl;
            std::cin >> filename;

            askForTransfer(target, filename);
        }

        if (op == 7) {
            waitingForReTransfer();
        }

        if (op == 8) {
            closesocket(clientSocket);
            closesocket(clientSocketRF);
            closesocket(clientSocketRM);
            WSACleanup();
            return;
        }
    }
}

int Client::logIn(std::string account, std::string pwd) {
    myName = account;
    myPwd  = pwd;

    auto pkg =
        PackageFactory::getInstance().createLoginPackage(myName.c_str(), myPwd);

    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout << "send successfully!" << std::endl;
    recv(clientSocket, buffer, MAX_BUFFER, 0);

    Parser parser;
    parser.parsePkgHead(buffer);
    std::cout << int(parser.info.opcode) << std::endl;
    parser.parseMsg(buffer);
    memset(buffer, 0, MAX_BUFFER);

    char type = char(*parser.msg.c_str());

    std::cout << parser.info.msglen << std::endl;
    std::cout << parser.info.account << std::endl;

    std::cout << "-----" << parser.msg << std::endl;
    if (type == 'a') {
        std::cout << "log in successfully!" << std::endl;

        if (account == "cc12345678") {
            logInRF("cc123456RF", "123456");
            logInSF("cc123456SF", "123456");
            logInRM("cc123456RM", "123456");
        }
        else if (account == "core123456") {
            logInRF("core1234RF", "123456");
            logInSF("core1234SF", "123456");
            logInRM("core1234RM", "123456");
        }
        else if (account == "godlike123") {
            logInRF("godlike_RF", "123456");
            logInSF("godlike_SF", "123456");
            logInRM("godlike_RM", "123456");
        }

        return 1;
    }
    else if (type == 'b') {
        std::cout << "your pwd is wrong!" << std::endl;
        return 2;
    }
    else if (type == 'c') {
        std::cout << "this account does not exist!" << std::endl;
        return 3;
    }

    return 0;
}

int Client::logInRF(std::string account, std::string pwd) {
    auto pkg =
        PackageFactory::getInstance().createLoginPackage(account.c_str(), pwd);

    std::cout << "xxx" << std::endl;

    send(clientSocketRF, pkg.start, pkg.size, 0);
    std::cout << "send successfully!" << std::endl;
    recv(clientSocketRF, buffer, MAX_BUFFER, 0);

    Parser parser;
    parser.parsePkgHead(buffer);
    std::cout << int(parser.info.opcode) << std::endl;
    parser.parseMsg(buffer);
    memset(buffer, 0, MAX_BUFFER);

    char type = char(*parser.msg.c_str());

    std::cout << "-----" << parser.msg << std::endl;
    if (type == 'a') {
        std::cout << "log in successfully!" << std::endl;
        return 1;
    }
    else if (type == 'b') {
        std::cout << "your pwd is wrong!" << std::endl;
        return 2;
    }
    else if (type == 'c') {
        std::cout << "this account does not exist!" << std::endl;
        return 3;
    }

    return 0;
}

int Client::logInSF(std::string account, std::string pwd) {
    auto pkg =
        PackageFactory::getInstance().createLoginPackage(account.c_str(), pwd);

    // std::cout << "xxx" << std::endl;

    send(clientSocketSF, pkg.start, pkg.size, 0);
    std::cout << "send successfully!" << std::endl;
    recv(clientSocketSF, buffer, MAX_BUFFER, 0);

    Parser parser;
    parser.parsePkgHead(buffer);
    std::cout << int(parser.info.opcode) << std::endl;
    parser.parseMsg(buffer);
    memset(buffer, 0, MAX_BUFFER);

    char type = char(*parser.msg.c_str());

    std::cout << "-----" << parser.msg << std::endl;
    if (type == 'a') {
        std::cout << "log in successfully!" << std::endl;
        return 1;
    }
    else if (type == 'b') {
        std::cout << "your pwd is wrong!" << std::endl;
        return 2;
    }
    else if (type == 'c') {
        std::cout << "this account does not exist!" << std::endl;
        return 3;
    }

    return 0;
}

int Client::logInRM(std::string account, std::string pwd) {
    auto pkg =
        PackageFactory::getInstance().createLoginPackage(account.c_str(), pwd);

    send(clientSocketRM, pkg.start, pkg.size, 0);
    std::cout << "send successfully!" << std::endl;
    recv(clientSocketRM, buffer, MAX_BUFFER, 0);

    Parser parser;
    parser.parsePkgHead(buffer);
    std::cout << int(parser.info.opcode) << std::endl;
    parser.parseMsg(buffer);
    memset(buffer, 0, MAX_BUFFER);

    char type = char(*parser.msg.c_str());

    std::cout << "-----" << parser.msg << std::endl;
    if (type == 'a') {
        std::cout << "log in successfully!" << std::endl;
        return 1;
    }
    else if (type == 'b') {
        std::cout << "your pwd is wrong!" << std::endl;
        return 2;
    }
    else if (type == 'c') {
        std::cout << "this account does not exist!" << std::endl;
        return 3;
    }

    return 0;
}

void Client::Msg2User(std::string target, std::string msg) {
    // std::cout<<"here 2"<<std::endl;
    auto pkg =
        PackageFactory::getInstance().createPackage2(myName, target, msg);
    std::cout << "pkg2 created successfully!" << std::endl;
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout << "send user successfully!" << std::endl;
    // recv(clientSocket, buffer, MAX_BUFFER, 0);
}

MsgInfo Client::readMsg() {
    memset(buffer_RM, 0, MAX_BUFFER);
    recv(clientSocketRM, buffer_RM, MAX_BUFFER, 0);
    Parser parser;
    // std::cout << "1234567777" << std::endl;
    parser.parsePkgHead(buffer_RM);
    parser.parseMsg(buffer_RM);
    std::cout << "the  new  msg is :   " << parser.msg << std::endl;

    return MsgInfo{parser.info.account, parser.info.target, parser.msg};
}

void Client::Msg2Group(std::string groupTarget, std::string msg) {
    auto pkg =
        PackageFactory::getInstance().createPackage3(myName, groupTarget, msg);
    std::cout << "pkg3 created successfully!" << std::endl;
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout << "send group msg successfully!" << std::endl;
}

void Client::transferFile(std::string target, std::string filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    std::cout << "file open successfully!" << std::endl;

    fseek(fp, 0, SEEK_SET);
    int NoPkg = 0;
    // 开始传文件
    int msgIndex = 0;
    int read_length;
    while (!feof(fp)) {
        memset(buffer, 0, MAX_BUFFER);
        read_length = fread((void*)buffer, 1, MAX_BUFFER - 40, fp);

        if (read_length == 0)
            break;

        std::string tem;
        NoPkg++;
        std::cout << "read_length: " << read_length << "   NO PKG: " << NoPkg << std::endl;
        tem.resize(read_length);
        memcpy(const_cast<char*>(tem.c_str()), buffer, read_length);

        memset(buffer, 0, MAX_BUFFER);

        auto pkg =
            PackageFactory::getInstance().createPackage4(myName, target, msgIndex, filename, tem);

        Parser parser;
        parser.parsePkgHead(pkg.start);
        parser.parseMsg(pkg.start + 40);

        // std::cout << "pkg_size: " << pkg.size << std::endl;
        send(clientSocket, pkg.start, pkg.size, 0);

        // std::cout << "file transferred successfully!" << std::endl;

        if (read_length < MAX_BUFFER - 40)
            break;

        msgIndex += read_length;
    }
    fclose(fp);
    std::cout << "file transfer is completed!" << std::endl;
}

void Client::recvFile() {
    memset(buffer_RF, 0, MAX_BUFFER + 1);
    int recv_count = recv(clientSocketRF, buffer_RF, MAX_BUFFER, 0);

    Parser parser;

    std::cout << "recv_count: " << recv_count << std::endl;

    parser.parsePkgHead(buffer_RF);
    parser.parseMsg(buffer_RF);

    auto save_path =
        std::filesystem::path(XSTR(ROOT_DIR)) / "file/recv_file.pdf";

    std::string filename = save_path.string();

    std::map<std::string, int>::iterator iter = fileIndex.find(filename);
    if (iter == fileIndex.end()) {
        fileIndex.insert(std::pair<std::string, int>(filename, 0));
    }

    // if (int(parser.info.opcode) == 4) {          // 非断传报文
    FILE* fp = fopen(filename.c_str(), "ab"); // 非接收重传文件为w

    // std::cout << "file opened successfully!" << std::endl;

    fwrite((char*)buffer_RF + 40, 1, parser.info.msglen, fp);
    fileIndex[filename] += recv_count - 40;

    // if (recv_count == MAX_BUFFER) {
    //     while ((recv_count = recv(clientSocketRF, buffer_RF, MAX_BUFFER, 0)) != 0) {
    //         fwrite((char*)buffer_RF + 40, 1, recv_count - 40, fp);
    //         fileIndex[filename] += recv_count - 40;
    //     }
    // }

    fclose(fp);
    std::cout << "file received successfully!" << std::endl;
    // }

    // if (int(parser.info.opcode) == 6) { // 断传报文
    //     // TODO: 路径名放到别的文件夹下可能要更改
    //     FILE* fp = fopen(filename.c_str(), "a"); // 非接收重传文件为w

    //     std::cout << "file opened successfully!" << std::endl;

    //     fwrite((char*)buffer_RF + 40, 1, recv_count, fp);
    //     fileIndex[filename] += recv_count;

    //     if (recv_count == MAX_BUFFER - 40) {
    //         while ((recv_count = recv(clientSocketRF, buffer_RF, MAX_BUFFER, 0)) != 0) {
    //             fwrite((char*)buffer_RF + 40, 1, recv_count, fp);
    //             fileIndex[filename] += recv_count;
    //         }
    //     }
    //     std::cout << "file received successfully!" << std::endl;
    // }
}

void Client::askForTransfer(std::string target, std::string filename) {
    if (fileIndex.find(filename) == fileIndex.end()) {
        fileIndex[filename] = 0;
    }
    auto pkg =
        PackageFactory::getInstance().createPackage6(myName, target, filename, fileIndex[filename]);
    send(clientSocket, pkg.start, pkg.size, 0);
    std::cout << "ask for retransfer successfully!" << std::endl;

    recvFile();
}

void Client::waitingForReTransfer() {
    memset(buffer, 0, MAX_BUFFER);
    int    recv_count = recv(clientSocket, buffer, MAX_BUFFER, 0);
    Parser parser;

    parser.parsePkgHead(buffer);
    parser.parseMsg(buffer);

    uint32_t    msgIndex = parser.info.msgindex;
    std::string target   = parser.info.account; // 收到的报文的发送者 是 需要文件的那一方
    std::string filename = parser.info.filename;

    std::cout << "pos 1  -----------" << std::endl;
    Sleep(5000);

    std::cout << "account  :  " << parser.info.account << std::endl;
    std::cout << "filename  :  " << parser.info.filename << std::endl;
    std::cout << "msgidx  :  " << parser.info.msgindex << std::endl;
    std::cout << "msglen  :  " << parser.info.msglen << std::endl;
    std::cout << "opcode  :  " << parser.info.opcode << std::endl;
    std::cout << "target  :  " << parser.info.target << std::endl;

    FILE* fp = fopen(parser.info.filename.c_str(), "r");
    for (int i = 0; i < parser.info.msgindex; ++i) { // 把指针指到需要传输的字节
        fread(buffer, 1, 1, fp);
        std::cout << "往后移动了" << i + 1 << "个位置" << std::endl;
    }

    std::cout << "pos 2  -----------" << std::endl;
    Sleep(5000);

    while (true) {
        int read_length = fread((void*)buffer, 1, MAX_BUFFER - 40, fp);

        std::cout << "file read successfully!" << std::endl;

        std::string tem;
        if (read_length != 0) {
            tem.resize(read_length);
            memcpy(const_cast<char*>(tem.c_str()), buffer, read_length);
        }

        memset(buffer, 0, MAX_BUFFER);

        std::cout << "pos 3  -----------" << std::endl;
        Sleep(5000);

        auto pkg =
            PackageFactory::getInstance().createPackage4(myName, target, msgIndex, filename, tem);
        if (read_length < MAX_BUFFER - 40) {
            send(clientSocket, pkg.start, pkg.size, 0);
            break;
        }
        send(clientSocket, pkg.start, pkg.size, 0);

        std::cout << "file transferred successfully!" << std::endl;

        msgIndex += read_length;
    }
    std::cout << "file transfer is completed!" << std::endl;
}
