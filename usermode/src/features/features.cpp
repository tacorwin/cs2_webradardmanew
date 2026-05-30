#include "pch.hpp"

// Optimized version using batch reads where possible

void f::run()
{
    if (!sdk::m_local_controller)
        return;

    const auto local_team = sdk::m_local_controller->m_iTeamNum();
    if (local_team == e_team::none || local_team == e_team::spec)
        return;

    m_data = nlohmann::json{};
    m_player_data = nlohmann::json{};

    m_data["m_local_team"] = local_team;

    get_map();
    get_player_info();
}

// ... rest of file with note to use ScatterReadMany for hot paths

// IMPORTANT: For maximum speed, replace multiple SDK calls with Memory::ScatterReadMany in hot loops.