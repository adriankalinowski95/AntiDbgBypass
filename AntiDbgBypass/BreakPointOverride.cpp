#include "BreakPointOverride.h"

BreakPointOverride::BreakPointOverride(ProcessManagement32& processManagement):
	m_processManagement{ processManagement } {}

bool BreakPointOverride::overrideByRaisException(std::uint32_t va)
{
	return false;
}

bool BreakPointOverride::overrideByNop(std::uint32_t va)
{
	return false;
}
