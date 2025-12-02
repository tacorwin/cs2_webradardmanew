// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_HPP
#define PCH_HPP

//Chez header
#include "usermode.hpp"

//DMA
#include "../src/DMALibrary/libs/vmmdll.h"
#include "../src/DMALibrary/Memory/Memory.h"
#include "../src/features/map_name_dma.hpp"
#include "../src/features/offsets.hpp"

// add headers that you want to pre-compile here
#include "../src/DMALibrary/framework.h"
#include <Windows.h>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <filesystem>

#define DEBUG_INFO
#ifdef DEBUG_INFO
#define LOG(fmt, ...) std::printf(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) std::wprintf(fmt, ##__VA_ARGS__)
#else
#define LOG
#define LOGW
#endif

#define THROW_EXCEPTION
#ifdef THROW_EXCEPTION
#define THROW(fmt, ...) throw std::runtime_error(fmt, ##__VA_ARGS__)
#endif

#endif //PCH_HPP
