#include "pch.hpp"

bool offsets::load()
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    std::string directory = exePath;
    directory = directory.substr(0, directory.find_last_of("\\/"));

    std::string path = directory + "\\offsets.json";

    std::ifstream file(path);
    if (!file.is_open())
        return false;

    nlohmann::json j;
    file >> j;
    file.close();

    auto& client = j["client.dll"];

    for (auto it = client.begin(); it != client.end(); ++it)
        client_offsets[it.key()] = it.value();

    return true;
}
