#pragma once
#include "ProcessStructures32Base.h"

class ProcessStructures32UM : public ProcessStructures32Base {
public:
	using ProcessStructures32Base::ProcessStructures32Base;
	std::optional<std::uint64_t> getPEBVa() override;
	PEBAbstraction* getPEBByVa(std::uint64_t va) override;
};