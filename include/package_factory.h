#pragma once

#include <stdint.h>
#include <string>

using Byte = char;

struct Package {
    /*
        head(40 byte):
            type    : 1
            account : 10
            target   : 10
            msgindex : 4
            msglen    : 2
            filename : 13
        content:
            
    */
    Byte*    start{nullptr};
    uint32_t size;
};


// Singleton & Factory
class PackageFactory {

public:

    static PackageFactory& getInstance() {
        static PackageFactory instance;
        return instance;
    }
    
    static Package createLoginPackage(const char* account /* 10 byte */, const std::string& password);
    static Package createPackage1(const char* account, char flag);
    static Package createPackage2(std::string account, std::string target, std::string msg );


    static void releasePackage(Package pkg);
};
