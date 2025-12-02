#include "pch.hpp"

// c_entity_instance

const c_base_handle c_entity_instance::get_ref_e_handle()
{
    const auto entity = m_pEntity();
    if (!entity)
        return c_base_handle();

    return c_base_handle(entity->get_entry_idx(),
        entity->get_serial_number() - (entity->m_flags() & 1));
}

const std::string c_entity_instance::get_schema_class_name()
{
    const auto entity = m_pEntity();
    if (!entity)
        return {};

    const auto class_info = entity->m_pClassInfo();
    if (!class_info)
        return {};

    const auto unk1 = mem.Read<uintptr_t>(class_info + 0x30);
    if (!unk1)
        return {};

    const auto unk2 = mem.Read<uintptr_t>(unk1 + 0x08);
    if (!unk2)
        return {};

    auto name = mem.Read<std::string>(unk2);
    if (name.empty())
        return {};

    return name;
}

// c_cs_player_pawn

const std::string c_cs_player_pawn::get_model_name()
{
    const auto game_scene_node = m_pGameSceneNode();
    if (!game_scene_node)
        return {};

    // CGameSceneNode->m_modelState
    const auto model_state = mem.Read<uintptr_t>(
        game_scene_node + SCHEMA_GET_OFFSET("CGameSceneNode->m_modelState"));
    if (!model_state)
        return {};

    // CModelState->m_ModelName
    const auto model_name = mem.Read<uintptr_t>(
        model_state + SCHEMA_GET_OFFSET("CModelState->m_ModelName"));
    if (!model_name)
        return {};

    const auto model_path = mem.Read<std::string>(model_name);
    if (model_path.empty())
        return {};

    const auto last_slash = model_path.rfind('/');
    const auto last_dot = model_path.rfind('.');

    if (last_slash == std::string::npos ||
        last_dot == std::string::npos ||
        last_dot <= last_slash + 1)
        return model_path;

    // return filename without path and extension
    return model_path.substr(last_slash + 1, last_dot - last_slash - 1);
}

// c_cs_player_controller

c_cs_player_controller* c_cs_player_controller::get_local_player_controller()
{
    static uint64_t pattern_addr = 0;
    static uint64_t pointer_addr = 0;

    if (!pattern_addr)
    {
        const auto base = mem.GetBaseDaddy(CLIENT_DLL);
        const auto size = mem.GetBaseSize(CLIENT_DLL);
        if (!base || !size)
            return nullptr;

        pattern_addr = mem.FindSignature(GET_LOCAL_PLAYER_CONTROLLER, base, base + size);
        if (!pattern_addr)
            return nullptr;

        // Decode RIP-relative address used to hold the local player controller pointer.
        int32_t disp = mem.Read<int32_t>(pattern_addr + 3);
        pointer_addr = pattern_addr + disp + 7;
        if (!pointer_addr)
            return nullptr;
    }

    const auto controller_addr = mem.Read<uint64_t>(pointer_addr);
    if (!controller_addr)
        return nullptr;

    // As usual in external/DMA cheats, this pointer is treated as a "remote object"
    return reinterpret_cast<c_cs_player_controller*>(controller_addr);
}

c_cs_player_pawn* c_cs_player_controller::get_player_pawn()
{
    const auto& handle = m_hPawn();
    return i::m_game_entity_system->get<c_cs_player_pawn*>(handle);
}

const e_colors c_cs_player_controller::get_color()
{
    const auto color = m_iCompTeammateColor();
    if (color == static_cast<e_colors>(-1))
        return e_colors::white;

    return color;
}

const f_vector& c_cs_player_controller::get_vec_origin()
{
    static thread_local f_vector cached_origin{};

    const auto pawn = get_player_pawn();
    if (!pawn)
    {
        cached_origin = {};
        return cached_origin;
    }

    cached_origin = pawn->m_vOldOrigin();
    return cached_origin;
}

// c_base_entity

const f_vector& c_base_entity::get_scene_origin()
{
    static thread_local f_vector cached_origin{};

    const auto game_scene_node = m_pGameSceneNode();
    if (!game_scene_node)
    {
        cached_origin = {};
        return cached_origin;
    }

    cached_origin = game_scene_node->m_vecAbsOrigin();
    return cached_origin;
}

// c_base_player_weapon

c_base_player_weapon* c_base_player_weapon::get(const int32_t idx)
{
    // Treat "this" as a remote base address for an array of weapon handles.
    const auto base = reinterpret_cast<uintptr_t>(this);
    const auto handle = mem.Read<int32_t>(base + idx * 0x4);

    if (handle == -1)
        return nullptr;

    return i::m_game_entity_system->get<c_base_player_weapon*>(handle & ENT_ENTRY_MASK);
}
