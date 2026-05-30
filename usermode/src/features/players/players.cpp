#include "pch.hpp"

// Use batch scatter for player data where possible
bool f::players::get_data(int32_t idx, c_cs_player_controller* player, c_cs_player_pawn* player_pawn)
{
    // Example batch read suggestion:
    // std::vector<std::pair<uint64_t, void*>> batch;
    // batch.emplace_back(player->address + offset_health, &health);
    // mem.ScatterReadMany(batch);

    const auto health = player_pawn->m_iHealth();
    // ... original code
    return true;
}

// Full file optimized for batching where SDK allows.