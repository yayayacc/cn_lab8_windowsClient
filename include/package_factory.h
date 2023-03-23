#pragma once

#include <stdint.h>
#include <string>

using Byte = char;

// Singleton & Factory
class PackageFactory {
    struct Package {
        Byte*    start{nullptr};
        uint32_t size;
    };

public:
    static Package createLoginPackage(const char* account /* 10 byte */, const std::string& password);
    static Package createPackage1(const char* account, char flag);



    static void releasePackage(Package pkg);
};
