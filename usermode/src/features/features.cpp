#include "pch.hpp"

// ------------------------------------------------------------
// f::run()
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// f::get_map()
// NOTE:
// CS2 no longer stores map name inside CGlobalVars.
// The correct location is a string pointer inside the hoststate.
// For now we safely fallback to <unknown> to avoid crashing.
// ------------------------------------------------------------

void f::get_map()
{
    std::string map = get_map_name();

    if (map.empty() || map.find("<empty>") != std::string::npos)
        m_data["m_map"] = "invalid";
    else
        m_data["m_map"] = map;
}


// ------------------------------------------------------------
// f::get_player_info()
// ------------------------------------------------------------

void f::get_player_info()
{
    m_data["m_players"] = nlohmann::json::array();

    const int32_t highest_idx = 2048; // safe upper bound

    for (int32_t idx = 0; idx < highest_idx; idx++)
    {
        auto entity = i::m_game_entity_system->get<c_base_entity*>(idx);
        if (!entity)
            continue;

        const auto handle = entity->get_ref_e_handle();
        if (!handle.is_valid())
            continue;

        const auto class_name = entity->get_schema_class_name();
        if (class_name.empty())
            continue;

        const auto hashed = fnv1a::hash(class_name);

        // ------------------------------------------------------------
        // Player controller
        // ------------------------------------------------------------
        if (hashed == fnv1a::hash("CCSPlayerController"))
        {
            auto controller = i::m_game_entity_system->get<c_cs_player_controller*>(handle);
            if (!controller)
                continue;

            auto pawn = controller->get_player_pawn();
            if (!pawn)
                continue;

            if (!f::players::get_data(idx, controller, pawn))
                continue;

            f::players::get_weapons(pawn);
            f::players::get_active_weapon(pawn);

            m_data["m_players"].push_back(m_player_data);
            continue;
        }

        // ------------------------------------------------------------
        // Dropped C4
        // ------------------------------------------------------------
        if (hashed == fnv1a::hash("C_C4"))
        {
            f::bomb::get_carried_bomb(entity);
            continue;
        }

        // ------------------------------------------------------------
        // Planted bomb
        // ------------------------------------------------------------
        if (hashed == fnv1a::hash("C_PlantedC4"))
        {
            auto planted = reinterpret_cast<c_planted_c4*>(entity);
            f::bomb::get_planted_bomb(planted);
            continue;
        }
    }
}
