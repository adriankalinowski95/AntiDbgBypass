#pragma once
#include "ProcessStructures32Base.h"

class ProcessStructures32Wow64UM : public ProcessStructures32Base {
public:
	using ProcessStructures32Base::ProcessStructures32Base;
	std::optional<PEB32> getPEB32();
	std::optional<std::uint32_t> getPEB32Va();
};