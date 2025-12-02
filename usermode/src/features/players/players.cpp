#include "pch.hpp"

bool f::players::get_data(int32_t idx, c_cs_player_controller* player, c_cs_player_pawn* player_pawn)
{
    const auto health = player_pawn->m_iHealth();
    const bool is_dead = (health <= 0);
    const auto vec_origin = player->get_vec_origin();
    const auto team = player->m_iTeamNum();

    m_player_data["m_idx"] = idx;
    m_player_data["m_name"] = player->m_sSanitizedPlayerName();
    m_player_data["m_color"] = player->get_color();
    m_player_data["m_team"] = team;
    m_player_data["m_health"] = health;
    m_player_data["m_is_dead"] = is_dead;
    m_player_data["m_model_name"] = player_pawn->get_model_name();
    m_player_data["m_steam_id"] = std::to_string(player->m_steamID());
    m_player_data["m_money"] = player->m_pInGameMoneyServices()->m_iAccount();
    m_player_data["m_armor"] = player_pawn->m_ArmorValue();

    m_player_data["m_position"]["x"] = vec_origin.m_x;
    m_player_data["m_position"]["y"] = vec_origin.m_y;

    m_player_data["m_eye_angle"] = player_pawn->m_angEyeAngles().m_y;
    m_player_data["m_has_helmet"] = player_pawn->m_pItemServices()->m_bHasHelmet();
    m_player_data["m_has_defuser"] = player_pawn->m_pItemServices()->m_bHasDefuser();
    m_player_data["m_weapons"] = nlohmann::json{};

    if (team == e_team::t && !is_dead)
        m_player_data["m_has_bomb"] =
        m_bomb_idx == (player->m_hPawn().get_entry_idx() & 0xffff);

    return true;
}


// -------------------------------------------------------------------------
// FIXED: DMA-SAFE WEAPON LIST EXTRACTION
// -------------------------------------------------------------------------
void f::players::get_weapons(c_cs_player_pawn* player_pawn)
{
    const auto weapon_services = player_pawn->m_pWeaponServices();
    if (!weapon_services)
        return;

    auto my_weapons = weapon_services->m_hMyWeapons();
    if (my_weapons.m_size == 0)
        return;

    std::set<std::string> utilities_set{};
    std::set<std::string> melee_set{};

    for (size_t i = 0; i < my_weapons.m_size; i++)
    {
        // DMA-SAFE WAY:
        c_base_handle handle = my_weapons.m_elements[i];
        if (!handle.is_valid())
            continue;

        auto weapon = i::m_game_entity_system->get<c_base_player_weapon*>(handle);
        if (!weapon)
            continue;

        auto weapon_data = weapon->m_WeaponData();
        if (!weapon_data)
            continue;

        std::string weapon_name = weapon_data->m_szName();
        if (weapon_name.empty())
            continue;

        if (weapon_name.size() > 7)
            weapon_name.erase(weapon_name.begin(), weapon_name.begin() + 7);

        e_weapon_type type = static_cast<e_weapon_type>(weapon_data->m_WeaponType());

        switch (type)
        {
        case e_weapon_type::submachinegun:
        case e_weapon_type::rifle:
        case e_weapon_type::shotgun:
        case e_weapon_type::sniper_rifle:
        case e_weapon_type::machinegun:
            m_player_data["m_weapons"]["m_primary"] = weapon_name;
            break;

        case e_weapon_type::pistol:
            m_player_data["m_weapons"]["m_secondary"] = weapon_name;
            break;

        case e_weapon_type::knife:
        case e_weapon_type::taser:
            melee_set.insert(weapon_name);
            break;

        case e_weapon_type::grenade:
            utilities_set.insert(weapon_name);
            break;

        default:
            break;
        }
    }

    m_player_data["m_weapons"]["m_melee"] =
        std::vector<std::string>(melee_set.begin(), melee_set.end());

    m_player_data["m_weapons"]["m_utilities"] =
        std::vector<std::string>(utilities_set.begin(), utilities_set.end());
}


// -------------------------------------------------------------------------
// FIXED ACTIVE WEAPON
// -------------------------------------------------------------------------
void f::players::get_active_weapon(c_cs_player_pawn* player_pawn)
{
    const auto weapon_services = player_pawn->m_pWeaponServices();
    if (!weapon_services)
        return;

    c_base_handle handle = weapon_services->m_hActiveWeapon();
    if (!handle.is_valid())
        return;

    auto active_weapon = i::m_game_entity_system->get<c_base_player_weapon*>(handle);
    if (!active_weapon)
        return;

    auto data = active_weapon->m_WeaponData();
    if (!data)
        return;

    std::string name = data->m_szName();
    if (name.empty())
        return;

    if (name.size() > 7)
        name.erase(name.begin(), name.begin() + 7);

    m_player_data["m_weapons"]["m_active"] = name;
}
