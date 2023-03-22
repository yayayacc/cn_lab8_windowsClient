#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>

class Client {
public:
    Client()  = default;
    ~Client() = default;

    SOCKET creatSocket(SOCKADDR_IN& hostAddr, char const* hostIP, char const* hostPort);
};
