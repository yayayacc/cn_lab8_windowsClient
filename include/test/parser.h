#pragma once

/*
    Only For Test
*/

#include <stdint.h>
#include <string>

struct LoginInfo {
    std::string account;
    std::string password;
};

struct Parser {
    // parsePkgHead first, then parseContent
    uint32_t parsePkgHead(char* start) {
        info.account.resize(10);

        // parse
        {
            char* cur = start;

            memcpy(const_cast<char*>(info.account.c_str()), cur + 1, 10);

            // info.account = buffer;

            memcpy(&pwd_len, cur + 1 + 10 + 15, 2);
        }

        return pwd_len;
    }

    void parseContent(char* start) {
        info.password.resize(pwd_len);
        memcpy(const_cast<char*>(info.password.c_str()), start, pwd_len);
    }

    LoginInfo info;
    uint32_t  pwd_len;
};