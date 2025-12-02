#pragma once
#include "pch.hpp"

//
// DMA-compatible re-implementation of CS2’s entity system paging layout.
// Works with entity handles, controller handles, pawn handles, etc.
//
class c_game_entity_system
{
public:
    // The REMOTE game address of the CGameEntitySystem instance
    uint64_t addr = 0;

public:

    // ------------------------------------------------------------
    // get<T>(idx) — return the remote pointer to entity instance
    // ------------------------------------------------------------
    template<typename T = c_base_entity*>
    T get(int32_t idx)
    {
        uint64_t ent_addr = get_entity_by_idx(idx);
        if (!ent_addr)
            return nullptr;

        uint64_t instance = mem.Read<uint64_t>(ent_addr);
        return reinterpret_cast<T>(instance);
    }

    // ------------------------------------------------------------
    // get<T>(handle) — same but using entity handle
    // ------------------------------------------------------------
    template<typename T = c_base_entity*>
    T get(const c_base_handle handle)
    {
        if (!handle.is_valid())
            return nullptr;

        return get<T>(handle.get_entry_idx());
    }

    // ------------------------------------------------------------
    // Debug helpers (optional)
    // ------------------------------------------------------------
    uint32_t get_entity_count() const
    {
        return mem.Read<uint32_t>(addr + 0x10);
    }

    uint64_t get_entity_list_root() const
    {
        return mem.Read<uint64_t>(addr + 0x8);
    }

private:

    // ------------------------------------------------------------
    // get_entity_by_idx — performs full entity paging lookup
    //
    // Original engine logic:
    //   topIndex  = idx >> 9        (0..63)
    //   subIndex  = idx & 0x1FF     (0..511)
    //
    // Entity page structure:
    //   rootPages = *(addr + 0x8)               → pointer to page directory
    //   pagePtr   = *(rootPages + topIndex*8)   → pointer to a 112-byte entry array
    //   entryPtr  = pagePtr + subIndex*112      → entity entry
    //
    // IMPORTANT:
    // We NEVER dereference remote pointers directly — always use mem.Read
    // ------------------------------------------------------------
    uint64_t get_entity_by_idx(int32_t idx)
    {
        if (idx < 0)
            return 0;

        uint32_t uidx = static_cast<uint32_t>(idx);

        // Guard checks from original code
        if (uidx >= 0x7FFE)
            return 0;

        uint32_t top_index = uidx >> 9;     // divide by 512
        if (top_index >= 0x3F)
            return 0;

        uint32_t sub_index = uidx & 0x1FF;  // modulo 512

        //
        // Read the root page table
        //
        uint64_t root_ptr = mem.Read<uint64_t>(addr + 0x8);
        if (!root_ptr)
            return 0;

        //
        // Read the specific page pointer
        //
        uint64_t page_ptr = mem.Read<uint64_t>(root_ptr + (top_index * 8));
        if (!page_ptr)
            return 0;

        //
        // Entity entry = page_ptr + (sub_index * 112)
        // The engine allocates each entry as a 112-byte structure.
        //
        uint64_t entity_entry = page_ptr + (sub_index * 112);

        return entity_entry;
    }
};
