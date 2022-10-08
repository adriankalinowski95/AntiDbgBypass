#pragma once
#include "ProcessManagement32.h"

class BreakPointOverride {
public:
	BreakPointOverride(ProcessManagement32& processManagement);
	bool overrideByRaisException(std::uint32_t va);
	bool overrideByNop(std::uint32_t va);

private:
	ProcessManagement32& m_processManagement;
};

