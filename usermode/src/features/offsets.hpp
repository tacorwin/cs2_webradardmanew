#pragma once
#include <string>
#include <unordered_map>

namespace offsets
{
    inline std::unordered_map<std::string, uint64_t> client_offsets;

    bool load(); // implemented below
}