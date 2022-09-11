#include "NtGlobalFlag32.h"


NtGlobalFlag32::NtGlobalFlag32(ProcessManagement32& processManagement):
				BaseBypass32{ processManagement } {}

bool NtGlobalFlag32::bypass() {
	if(isNtGlobalFlag32()) {
		if(!bypassNtGlobalFlag32()) {
			return false;
		}
	}

	return true;
}

bool NtGlobalFlag32::isNtGlobalFlag32() {
	auto peb = m_processManagement.getStructures().getPEB();
	if(!peb) {
		return false;
	}

	return peb->getNtGlobalFlag() & NT_GLOBAL_FLAG_DEBUGGED;
}

bool NtGlobalFlag32::bypassNtGlobalFlag32() {
	auto peb = m_processManagement.getStructures().getPEB();
	auto pebVa = m_processManagement.getStructures().getPEBVa();
	if(!peb || !pebVa) {
		return false;
	}

	auto currFlag = peb->getNtGlobalFlag();
	currFlag &= ~NT_GLOBAL_FLAG_DEBUGGED;

	auto flagAddress = *pebVa + peb->getNtGlobalFlagOffset();

	return m_processManagement.getVmm().putVar(currFlag, flagAddress);
}