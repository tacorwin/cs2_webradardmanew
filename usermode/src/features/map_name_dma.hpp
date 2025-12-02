#pragma once
#include "pch.hpp"
#include "offsets.hpp"  // for offsets::client_offsets

inline std::string get_map_name()
{
    // 1) Get client.dll base using your DMA Memory API
    const uint64_t clientBase = mem.GetBaseDaddy("client.dll");
    if (!clientBase)
        return {};

    // 2) Get dwGlobalVars RVA from offsets.json
    auto it = offsets::client_offsets.find("dwGlobalVars");
    if (it == offsets::client_offsets.end())
        return {};

    const uint64_t globalVarsPtr = clientBase + it->second;

    // 3) Read the pointer to CGlobalVars
    const uint64_t globalVars = mem.Read<uint64_t>(globalVarsPtr);
    if (!globalVars)
        return {};

    // 4) At CGlobalVars + 0x18 lives a char* to the map name
    const uint64_t namePtr = mem.Read<uint64_t>(globalVars + 0x18);
    if (!namePtr)
        return {};

    // 5) Read the string data into a buffer using your bool Read(...)
    char buffer[64]{};
    if (!mem.Read(namePtr, buffer, sizeof(buffer)))
        return {};

    // Ensure null-termination just in case
    buffer[sizeof(buffer) - 1] = '\0';

    return std::string(buffer);
}
