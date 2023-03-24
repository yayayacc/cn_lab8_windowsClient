#pragma once

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include"package_factory.h"
#include"parser.h"

#define MAX_BUFFER 4095

class Client {
public:
    Client()  = default;
    ~Client() = default;

    void creatSocket(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort);

    void run();


    // 输入为账户名和密码， 输出为1登录成功，2密码错误，3账户名不存在
    int logIn(std::string account, std::string pwd);

    // 输入为目标账户名 和 消息内容
    void Msg2User(std::string target, std::string msg);
    void Msg2Group(std::string groupTarget, std::string msg);
    void readMsg();


    SOCKET clientSocket;
    char   buffer[MAX_BUFFER];

    std::string myName;
    std::string myPwd;
};
