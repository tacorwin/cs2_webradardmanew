#pragma once

#include <string>
#include <cstdint>
#include "DMALibrary/Memory/Memory.h"
#include "ext/fnv1a/fnv1a.hpp"

// Global DMA memory instance (declared inline in Memory.cpp)
extern Memory mem;


// ---------------------------------------------------------
//  SCHEMA OFFSET LOOKUP
// ---------------------------------------------------------
#define SCHEMA_GET_OFFSET(field) \
    schema::get_offset(fnv1a::hash_const(field))



// ---------------------------------------------------------
//  BASIC OFFSET READ (NO FIELD NAME)
// ---------------------------------------------------------
#define SCHEMA_ADD_OFFSET(type, name, offset)                     \
    inline type name() const                                      \
    {                                                              \
        return mem.Read<type>(                                    \
            reinterpret_cast<uintptr_t>(this) + offset);           \
    }



// ---------------------------------------------------------
//  STRING READ WITH RAW OFFSET (NO FIELD NAME)
// ---------------------------------------------------------
#define SCHEMA_ADD_STRING_OFFSET(name, offset)                    \
    inline std::string name() const                               \
    {                                                              \
        uintptr_t str_ptr = mem.Read<uintptr_t>(                  \
            reinterpret_cast<uintptr_t>(this) + offset);           \
        return mem.Read<std::string>(str_ptr);                     \
    }



// ---------------------------------------------------------
//  FIELD READ (LOOKUP OFFSET BY FIELD NAME VIA SCHEMA SYSTEM)
// ---------------------------------------------------------
#define SCHEMA_ADD_FIELD_OFFSET(type, name, field, extra)         \
    inline type name() const                                      \
    {                                                              \
        uintptr_t addr =                                           \
            reinterpret_cast<uintptr_t>(this) +                    \
            schema::get_offset(fnv1a::hash_const(field)) + extra;  \
        return mem.Read<type>(addr);                               \
    }

#define SCHEMA_ADD_FIELD(type, name, field)                       \
    SCHEMA_ADD_FIELD_OFFSET(type, name, field, 0)



// ---------------------------------------------------------
//  STRING FIELD READ (LOOKUP OFFSET BY FIELD NAME)
// ---------------------------------------------------------
#define SCHEMA_ADD_STRING(name, field)                             \
    inline std::string name() const                                \
    {                                                               \
        uintptr_t ptr = mem.Read<uintptr_t>(                       \
            reinterpret_cast<uintptr_t>(this) +                    \
            schema::get_offset(fnv1a::hash_const(field)));         \
        return mem.Read<std::string>(ptr);                          \
    }



// ---------------------------------------------------------
//  SCHEMA NAMESPACE DECLARATION
// ---------------------------------------------------------
namespace schema
{
    bool setup();
    uint32_t get_offset(fnv1a_t hashed_field_name);
}
