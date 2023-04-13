#pragma once

#include <string>
#include <unordered_map>

struct AppDataPack {
    std::string selected_item;
    bool        is_group{false};
    std::string input_buffer;

    std::unordered_map<std::string, std::string> msg_record;
};