#include <iostream>
#include <string>

#include "client.h"
#include "gui.h"
#include "package_factory.h"
#include "test/parser.h"

// int main() {
//     GUI gui;
//     gui.run();
// }

int main() {
    // WSADATA     wsadata;
    // SOCKADDR_IN servADDR;

    // if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
    //     std::cout << "wsa start up failed!" << std::endl;
    //     exit(1);
    // }

    // Client      client;
    // char const* hostIP   = "192.168.1.103"; // TODO:这一部分后面需要更改，可能需要改成NAT穿透
    // char const* hostPort = "6666";

    // client.creatSocket(servADDR, hostIP, hostPort);
    // // 到这一行就已经完成了套接字的创建，接下来就是链接目标主机进行登录的部分了。
    // connect(client.clientSocket, (SOCKADDR*)&servADDR, sizeof(servADDR));
    // std::cout << "connect successfully!" << std::endl;
    // client.run();

    {
        auto pkg =
            PackageFactory::createLoginPackage("1234567890", "password");

        Parser parser;

        uint32_t ret = parser.parsePkgHead(pkg.start);
        parser.parseContent(pkg.start + 40);

        std::cout << parser.info.account << std::endl;
        std::cout << parser.info.password << std::endl;

        PackageFactory::releasePackage(pkg);
    }

    return 0;
}