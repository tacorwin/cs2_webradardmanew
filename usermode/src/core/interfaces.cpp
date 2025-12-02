#include "pch.hpp"

bool i::setup()
{
    bool success = true;

    // Get CLIENT DLL info via DMA
    uint64_t client_base = mem.GetBaseDaddy("client.dll");
    uint64_t client_size = mem.GetBaseSize("client.dll");
    if (!client_base || !client_size)
        return false;

    //
    // 1. Schema System
    //
    uint64_t sig_schema = mem.FindSignature(GET_SCHEMA_SYSTEM, client_base, client_base + client_size);
    if (!sig_schema)
        return false;

    // rip resolver (DMA-safe)
    int32_t disp_schema = mem.Read<int32_t>(sig_schema + 3);
    uint64_t schema_addr = sig_schema + disp_schema + 7;

    m_schema_system = mem.Read<c_schema_system*>(schema_addr);
    success &= (m_schema_system != nullptr);

    //
    // 2. Global Vars
    //
    uint64_t sig_gvars = mem.FindSignature(GET_GLOBAL_VARS, client_base, client_base + client_size);
    if (!sig_gvars)
        return false;

    int32_t disp_gvars = mem.Read<int32_t>(sig_gvars + 3);
    uint64_t gvars_addr = sig_gvars + disp_gvars + 7;

    m_global_vars = mem.Read<c_global_vars*>(gvars_addr);
    success &= (m_global_vars != nullptr);

    //
    // 3. Game Entity System
    //
    uint64_t sig_entity = mem.FindSignature(GET_ENTITY_LIST, client_base, client_base + client_size);
    if (!sig_entity)
        return false;

    int32_t disp_entity = mem.Read<int32_t>(sig_entity + 3);
    uint64_t entity_addr = sig_entity + disp_entity + 7;

    m_game_entity_system = mem.Read<c_game_entity_system*>(entity_addr);
    success &= (m_game_entity_system != nullptr);

    return success;
}
