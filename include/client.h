#pragma once

#include <Windows.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "package_factory.h"
#include "parser.h"

#define MAX_BUFFER 4095

struct MsgInfo {
    std::string account;
    std::string target;
    std::string msg;
};

class Client {
public:
    Client()  = default;
    ~Client() = default;

    void createSocket(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort);
    void createSocketRM(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort);
    void createSocketRF(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort);

    void run();

    // 输入为账户名和密码， 输出为1登录成功，2密码错误，3账户名不存在
    int logIn(std::string account, std::string pwd);
    int logInRF(std::string account, std::string pwd);
    int logInRM(std::string account, std::string pwd);

    // 输入为目标账户名 和 消息内容
    void    Msg2User(std::string target, std::string msg);
    void    Msg2Group(std::string groupTarget, std::string msg);
    MsgInfo readMsg();
    void    transferFile(std::string target, std::string filename);
    void    recvFile();
    void    askForTransfer(std::string target, std::string filename);
    void    waitingForReTransfer();

    SOCKET clientSocket;
    SOCKET clientSocketRF;
    SOCKET clientSocketRM;

    char buffer[MAX_BUFFER];

    std::string                myName;
    std::string                myPwd;
    std::map<std::string, int> fileIndex; // 用于存储每个文件传了多长
};
