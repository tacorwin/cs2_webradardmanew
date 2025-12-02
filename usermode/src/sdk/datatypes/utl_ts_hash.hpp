#pragma once
#include "pch.hpp"

extern Memory mem;

class c_utl_memory_pool
{
public:
    int32_t size() const
    {
        return this->m_blocks_allocated;
    }

private:
    uint8_t m_pad0[0x0C];        // 0x00
    int32_t m_blocks_allocated;  // 0x0C
};
static_assert(sizeof(c_utl_memory_pool) == 0x10, "wrong size on c_utl_memory_pool");

template<class T, int Count, typename K = uintptr_t>
class c_utl_ts_hash
{
public:
    uint32_t size() const
    {
        return m_entry_memory.size();
    }

    uint32_t get_elements(int32_t first_element, uint32_t size, uintptr_t* handles) const
    {
        uint32_t element_idx = 0;

        for (uint32_t i = 0; i < Count; i++)
        {
            const hash_bucket_t& bucket = m_buckets[i];
            auto element = bucket.m_first_uncommitted;

            while (element)
            {
                if (--first_element >= 0)
                {
                    element = element->m_next();
                    continue;
                }

                handles[element_idx++] = reinterpret_cast<uintptr_t>(element);
                if (element_idx >= size)
                    return element_idx;

                element = element->m_next();
            }
        }

        return element_idx;
    }

    T operator[](uintptr_t hash)
    {
        auto* ptr = reinterpret_cast<hash_fixed_data_internal_t<T>*>(hash);
        return ptr->m_data();
    }

private:
    template<typename D>
    struct hash_fixed_data_internal_t
    {
        D m_data()
        {
            return mem.Read<D>(reinterpret_cast<uintptr_t>(this) + 0x10);
        }

        hash_fixed_data_internal_t<D>* m_next()
        {
            return mem.Read<hash_fixed_data_internal_t<D>*>(reinterpret_cast<uintptr_t>(this) + 0x08);
        }
    };

    struct hash_bucket_t
    {
        uint8_t m_pad0[0x08];
        hash_fixed_data_internal_t<T>* m_first;            // 0x08
        hash_fixed_data_internal_t<T>* m_first_uncommitted; // 0x10
    };

private:
    c_utl_memory_pool m_entry_memory;     // 0x00
    uint8_t m_pad0[0x70];                 // 0x10
    hash_bucket_t m_buckets[Count];       // 0x80
};
