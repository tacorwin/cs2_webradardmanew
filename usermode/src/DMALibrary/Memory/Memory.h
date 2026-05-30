#pragma once
#include "../src/pch.hpp"
#include "../src/DMALibrary/libs/vmmdll.h"
#include "../src/DMALibrary/Memory/InputManager.h"
#include "../src/DMALibrary/Memory/Registry.h"
#include "../src/DMALibrary/Memory/Shellcode.h"

class Memory
{
private:
	struct LibModules
	{
		HMODULE VMM = nullptr;
		HMODULE LeechCore = nullptr;
		HMODULE FTD3XX = nullptr;
	} modules;

	VMM_HANDLE vHandle = nullptr;

public:
	Memory();
	~Memory();

	bool Init();

	// Original Read
	template <typename T>
	T Read(uint64_t address, int pid = 0) const {
		T value{};
		DWORD bytesRead = 0;
		VMMDLL_MemReadEx(vHandle, pid ? pid : current_process.PID, address, (PBYTE)&value, sizeof(T), &bytesRead, VMMDLL_FLAG_NOCACHE);
		return value;
	}

	// NEW HIGH PERFORMANCE BATCH READ
	template <typename T>
	bool ScatterReadMany(const std::vector<std::pair<uint64_t, T*>>& requests, int pid = 0) {
		if (requests.empty()) return true;
		VMMDLL_SCATTER_HANDLE hScatter = VMMDLL_Scatter_Initialize(pid ? pid : current_process.PID);
		if (!hScatter) return false;

		for (const auto& [addr, ptr] : requests) {
			VMMDLL_Scatter_Prepare(hScatter, addr, sizeof(T));
		}

		VMMDLL_Scatter_Execute(hScatter);

		for (const auto& [addr, ptr] : requests) {
			VMMDLL_Scatter_Read(hScatter, addr, sizeof(T), (PBYTE)ptr);
		}
		VMMDLL_Scatter_Close(hScatter);
		return true;
	}

	bool ReadLarge(uint64_t address, void* buffer, size_t size, int pid = 0) const {
		DWORD read_size = 0;
		if (!VMMDLL_MemReadEx(vHandle, pid ? pid : current_process.PID, address, static_cast<PBYTE>(buffer), size, &read_size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_ZEROPAD_ON_FAIL)) {
			return false;
		}
		return read_size == size;
	}
};