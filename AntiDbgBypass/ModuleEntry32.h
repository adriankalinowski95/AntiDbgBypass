#pragma once
#include "ModuleEntryBase.h"
#include <TlHelp32.h>

class ModuleEntry32 : public ModuleEntryBase<MODULEENTRY32> {
public:
	using ModuleEntryBase<MODULEENTRY32>::ModuleEntryBase;

	std::uint64_t getModuleId() override {
		return m_moduleEntry.th32ModuleID;
	}

	std::uint64_t getProcessId() override {
		return m_moduleEntry.th32ProcessID;
	}
};