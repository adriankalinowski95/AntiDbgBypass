#pragma once
#include "ProcessManagement32.h"
#include "ProcessManagementUM.h"

class AntiDbg {
public:
	AntiDbg(std::string processName);
	bool isNtGlobalFlag32On();
	bool bypassNtGlobalFlagCheck();
private:
	ProcessManagementUM m_rocessManagementUM;
	ProcessManagement32 m_processManagement32;

	static constexpr std::uint32_t FLG_HEAP_ENABLE_TAIL_CHECK = 0x10;
	static constexpr std::uint32_t FLG_HEAP_ENABLE_FREE_CHECK = 0x20;
	static constexpr std::uint32_t FLG_HEAP_VALIDATE_PARAMETERS = 0x40;
	static constexpr std::uint32_t NT_GLOBAL_FLAG_DEBUGGED =  (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS);
};

