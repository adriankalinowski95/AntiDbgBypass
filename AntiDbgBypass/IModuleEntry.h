#pragma once
#include <cstdint>
#include <wtypes.h>

class IModuleEntry {
public:
	virtual std::uint64_t getDwSize() = 0;
	virtual std::uint64_t getModuleId() = 0;
	virtual std::uint64_t getProcessId() = 0;
	virtual std::uint64_t getGlblcntUsage() = 0;
	virtual std::uint64_t getProccentUsage() = 0;
	virtual std::uint64_t getModBaseVa() = 0;
	virtual std::uint64_t getModBaseSize() = 0;
	virtual HMODULE getHModule() = 0;
};