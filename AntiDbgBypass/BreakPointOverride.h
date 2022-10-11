#pragma once
#include "ProcessManagement32.h"

class BreakPointOverride {
public:
	BreakPointOverride(ProcessManagement32& processManagement);
	bool overrideByRaisException(std::uint32_t va);
	bool overrideByNop(std::uint32_t va);

private:
	ProcessManagement32& m_processManagement;

	std::optional<std::uint32_t> getRaiseExceptionVa();
	std::vector<std::uint8_t> getRaiseExceptionTrampoline(std::uint32_t raiseExceptionVa, std::uint32_t bpVa);
	bool overrideUsingTrampoline(std::vector<std::uint8_t>& trapomline);
};