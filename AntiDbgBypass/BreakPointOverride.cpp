#include "BreakPointOverride.h"
#include "FunctionCallerBuilder.h"

BreakPointOverride::BreakPointOverride(ProcessManagement32& processManagement):
	m_processManagement{ processManagement } {}

bool BreakPointOverride::overrideByRaisException(std::uint32_t va) {
	if (!isVaInCodeRange(va)) {
		return false;
	}
	
	return false;
}

bool BreakPointOverride::overrideByNop(std::uint32_t va) {
	if (!isVaInCodeRange(va)) {
		return false;
	}

	std::uint8_t nop = 0x90;
	
	return m_processManagement.getVmm().putVar(nop, va);
}

bool BreakPointOverride::isVaInCodeRange(std::uint32_t va)
{
	auto moduleEntry = m_processManagement.getStructures().getModuleEntry();
	if (!moduleEntry) {
		return false;
	}

	auto range = moduleEntry->getCodeRange();
	if (!range) {
		return false;
	}

	return va >= range->first && va <= range->second;
}

std::optional<std::uint32_t> BreakPointOverride::getRaiseExceptionVa()
{
	return std::optional<std::uint32_t>();
}

std::vector<std::uint8_t> BreakPointOverride::getRaiseExceptionTrampoline(std::uint32_t raiseExceptionVa, std::uint32_t bpVa)
{
	return std::vector<std::uint8_t>();
}

bool BreakPointOverride::overrideUsingTrampoline(std::vector<std::uint8_t>& trapomline)
{
	return false;
}

