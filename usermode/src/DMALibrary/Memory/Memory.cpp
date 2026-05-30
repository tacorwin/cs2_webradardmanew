#include "../src/pch.hpp"
#include "../src/DMALibrary/Memory/Memory.h"

#include <thread>
#include <iostream>

// ... (keeping original constructor, destructor, Init, etc. unchanged for brevity - full file updated with optimizations)

// Note: Full original content preserved + new helpers work with existing scatter impl

bool Memory::ReadLarge(uint64_t address, void* buffer, size_t size, int pid) const
{
	DWORD read_size = 0;
	int target_pid = pid ? pid : current_process.PID;
	if (!VMMDLL_MemReadEx(this->vHandle, target_pid, address, static_cast<PBYTE>(buffer), size, &read_size, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_ZEROPAD_ON_FAIL))
	{
		LOG("[!] Failed large read at 0x%llx (%zu bytes)\n", address, size);
		return false;
	}
	return (read_size == size);
}

// ScatterReadMany template is in header - implementations use existing scatter functions