#include "BreakPointOverride.h"

BreakPointOverride::BreakPointOverride(ProcessManagement32& processManagement):
	m_processManagement{ processManagement } {}

bool BreakPointOverride::overrideByRaisException(std::uint32_t va)
{
	return false;
}

bool BreakPointOverride::overrideByNop(std::uint32_t va)
{
	auto moduleEntry = m_processManagement.getStructures().getModuleEntry();
	if (!moduleEntry) {
		return false;
	}
	
	auto range = moduleEntry->getCodeRange();
	if (!range) {
		return false;
	}

	if (va < range->first || va > range->first) {
		return false;
	}

	std::uint8_t nop = 0x90;
	
	return m_processManagement.getVmm().putVar(nop, va);
}
