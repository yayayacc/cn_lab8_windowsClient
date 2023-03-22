#pragma once

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER 4095

class Client {
public:
    Client()  = default;
    ~Client() = default;

    void creatSocket(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort);

    void run();
    bool logIn();

    SOCKET clientSocket;
    char   buffer[MAX_BUFFER];
};
