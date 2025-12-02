#pragma once
#include "pch.hpp"

class c_schema_class_field_data
{
public:
    uint64_t addr = 0;

    SCHEMA_ADD_STRING_OFFSET(m_name, 0x00);
    SCHEMA_ADD_OFFSET(uint16_t, m_single_inheritance_offset, 0x10);
};

class c_schema_type_declared_class
{
public:
    uint64_t addr = 0;

    SCHEMA_ADD_STRING_OFFSET(m_binary_name, 0x08);

    std::pair<uint16_t, c_schema_class_field_data*> get_fields() const
    {
        uint16_t count = mem.Read<uint16_t>(addr + 0x1C);
        uint64_t ptr = mem.Read<uint64_t>(addr + 0x28);
        return { count, reinterpret_cast<c_schema_class_field_data*>(ptr) };
    }
};

class c_schema_system_type_scope
{
public:
    uint64_t addr = 0;

    SCHEMA_ADD_STRING_OFFSET(m_module_name, 0x08);

    auto m_hash_classes() const
    {
        return mem.Read<
            c_utl_ts_hash<c_schema_type_declared_class*, 256, uint32_t>
        >(addr + 0x500);
    }
};

class c_schema_system
{
public:
    uint64_t addr = 0;

    c_schema_system_type_scope* find_type_scope_for_module(std::string_view module) const
    {
        uint32_t size = mem.Read<uint32_t>(addr + 0x188);
        if (!size || size > 50)
            return nullptr;

        uint64_t list_ptr = mem.Read<uint64_t>(addr + 0x190);
        if (!list_ptr)
            return nullptr;

        std::vector<uint64_t> list(size);
        mem.Read(list_ptr, list.data(), size * sizeof(uint64_t));

        for (uint64_t scope_addr : list)
        {
            auto scope = reinterpret_cast<c_schema_system_type_scope*>(scope_addr);
            std::string name = scope->m_module_name();
            if (!name.empty() && name.find(module) != std::string::npos)
                return scope;
        }

        return nullptr;
    }
};