#pragma once
#include <cstdint>
#include <wtypes.h>
#include <utility>
#include <optional>

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
	
	virtual std::optional<std::pair<std::uint64_t, std::uint64_t>> getCodeRange() = 0;
};