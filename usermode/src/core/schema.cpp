#include "pch.hpp"

// global DMA instance
extern Memory mem;

struct schema_data_t
{
    fnv1a_t m_hashed_field_name = 0;
    uint32_t m_offset = 0;
};

static std::vector<schema_data_t> m_schema_data{};

bool schema::setup()
{
    const auto start = std::chrono::high_resolution_clock::now();

    // Locate type scope for client.dll
    auto type_scope = i::m_schema_system->find_type_scope_for_module(CLIENT_DLL);
    if (!type_scope)
        return false;

    auto hash_classes = type_scope->m_hash_classes();
    const uint32_t table_size = hash_classes.size();

    LOG_INFO("found '%d' schema classes in module '%s'", table_size, CLIENT_DLL);

    // Allocate array for remote hash entries
    std::unique_ptr<uintptr_t[]> elements(new uintptr_t[table_size]);

    // Populate array of remote entries
    const uint32_t elements_size = hash_classes.get_elements(0, table_size, elements.get());

    for (uint32_t idx = 0; idx < elements_size; idx++)
    {
        const uintptr_t entry = elements[idx];
        if (!entry)
            continue;

        // Convert remote pointer → declared class
        c_schema_type_declared_class* class_binding =
            hash_classes[entry];

        if (!class_binding)
            continue;

        // Read schema fields
        auto [num_fields, schema_field] = class_binding->get_fields();
        if (!schema_field || num_fields == 0)
            continue;

        for (uint32_t f = 0; f < num_fields; f++)
        {
            if (!schema_field)
                break;

            // Build "Class->Field" string
            const std::string class_name = class_binding->m_binary_name();
            const std::string field_name = schema_field->m_name();

            std::string full = class_name + "->" + field_name;

            // Store hashed name → offset
            m_schema_data.push_back(
                {
                    fnv1a::hash(full),
                    schema_field->m_single_inheritance_offset()
                }
            );

            // Advance to next field in memory
            schema_field = reinterpret_cast<c_schema_class_field_data*>(
                reinterpret_cast<uintptr_t>(schema_field) +
                sizeof(c_schema_class_field_data)
            );
        }
    }

    return !m_schema_data.empty();
}


uint32_t schema::get_offset(const fnv1a_t hashed_field_name)
{
    auto it = std::ranges::find_if(
        m_schema_data,
        [&](const schema_data_t& d)
        {
            return d.m_hashed_field_name == hashed_field_name;
        }
    );

    if (it != m_schema_data.end())
        return it->m_offset;

    LOG_ERROR("failed to find an offset for the field with the hash value '%d'", hashed_field_name);
    return 0;
}