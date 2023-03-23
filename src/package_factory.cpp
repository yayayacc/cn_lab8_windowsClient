#include "package_factory.h"

void PackageFactory::releasePackage(Package pkg) {
    if (pkg.start) delete[] pkg.start;
}

PackageFactory::Package PackageFactory::createLoginPackage(const char* account /* 10 byte */, const std::string& password) {
    Package pkg;
    pkg.size  = 40 + password.size(); // 40 Byte is the size of PackageHead
    pkg.start = new Byte[pkg.size];

    // clear
    memset(pkg.start, 0, pkg.size);

    /*
        head(40 byte):
            type    : 1
            account : 10
            pad0    : 10 + 5
            pwd_len : 2
            pad1    : 12
        content:
            pwd
    */

    // copying
    {
        Byte* cur = pkg.start;

        // 1.type
        Byte type = 10;
        memcpy(pkg.start, &type, 1);

        // 2.account
        memcpy(pkg.start + 1, account, 10);

        // 3.pwd_len
        size_t pwd_len = password.size();
        memcpy(pkg.start + 1 + 10 + 15, &pwd_len, 2);

        // 4.pwd_content
        memcpy(pkg.start + 40, password.c_str(), password.size());
    }

    return pkg;
}