#include "NtGlobalFlag32.h"


NtGlobalFlag32::NtGlobalFlag32(ProcessManagement32& processManagement):
				BaseBypass32(processManagement) {}

bool NtGlobalFlag32::bypass() {
	if(isNtGlobalFlag32()) {
		if(!bypassNtGlobalFlag32()) {
			return false;
		}
	}
	if(isNtGlobalFlag64()) {
		if(!bypassNtGlobalFlag64()) {
			return false;
		}
	}

	return true;
}

bool NtGlobalFlag32::isNtGlobalFlag32() {
	auto peb = m_processManagement.getPEB32();
	if(!peb) {
		return false;
	}

	return peb->NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED;
}

bool NtGlobalFlag32::isNtGlobalFlag64() {
	auto peb = m_processManagement.getPEB64();
	if(!peb) {
		return false;
	}

	return peb->NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED;
}

bool NtGlobalFlag32::bypassNtGlobalFlag32() {
	auto pbi = m_processManagement.getPBI();
	auto peb = m_processManagement.getPEB32();
	if(!peb || !pbi) {
		return false;
	}

	auto currFlag = peb->NtGlobalFlag;
	currFlag &= ~NT_GLOBAL_FLAG_DEBUGGED;

	auto flagAddress = (std::uint32_t)m_processManagement.getPEB32FromPBI(*pbi) + offsetof(PEB32, NtGlobalFlag);

	return m_processManagement.getVmm().putVar(currFlag, flagAddress);
}

bool NtGlobalFlag32::bypassNtGlobalFlag64() {
	auto pbi = m_processManagement.getPBI();
	auto peb = m_processManagement.getPEB64();
	if(!peb || !pbi) {
		return false;
	}

	auto currFlag = peb->NtGlobalFlag;
	currFlag &= ~NT_GLOBAL_FLAG_DEBUGGED;

	auto flagAddress = (std::uint32_t)pbi->PebBaseAddress + offsetof(PEB64, NtGlobalFlag);

	return m_processManagement.getVmm().putVar(currFlag, flagAddress);
}
