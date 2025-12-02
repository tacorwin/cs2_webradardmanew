#pragma once
#include "pch.hpp"

// Represents CS2's CGlobalVars (remote object).
// The pointer value of c_global_vars* is the game address; we never
// dereference it directly, only via mem.Read in the SCHEMA macros.

class c_global_vars
{
public:
    SCHEMA_ADD_OFFSET(float, m_curtime, 0x30);
    SCHEMA_ADD_STRING_OFFSET(m_map_name, 0x180);
};
