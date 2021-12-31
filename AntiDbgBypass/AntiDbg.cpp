#include "AntiDbg.h"
#include <cstddef>

AntiDbg::AntiDbg(std::string processName):
	m_rocessManagementUM{ processName },
	m_processManagement32{ m_rocessManagementUM } {
	}

bool AntiDbg::isNtGlobalFlag32() {
	auto peb = m_processManagement32.getPEB32();
	if(!peb) {
		return false;
	}
	
	return peb->NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED;
}

bool AntiDbg::bypassNtGlobalFlag32() {
	auto pbi = m_processManagement32.getPBI();
	auto peb = m_processManagement32.getPEB32();
	if(!peb || !pbi) {
		return false;
	}
	
	auto currFlag = peb->NtGlobalFlag;
	currFlag &= ~NT_GLOBAL_FLAG_DEBUGGED;

	auto flagAddress = (std::uint32_t)m_processManagement32.getPEB32FromPBI(*pbi) + offsetof(PEB32, NtGlobalFlag);

	return m_processManagement32.getVmm().putVar(currFlag, flagAddress);
}

bool AntiDbg::isNtGlobalFlag64() {
	auto peb = m_processManagement32.getPEB64();
	if(!peb) {
		return false;
	}

	return peb->NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED;
}

bool AntiDbg::bypassNtGlobalFlag64() {
	auto pbi = m_processManagement32.getPBI();
	auto peb = m_processManagement32.getPEB64();
	if(!peb || !pbi) {
		return false;
	}

	auto currFlag = peb->NtGlobalFlag;
	currFlag &= ~NT_GLOBAL_FLAG_DEBUGGED;

	auto flagAddress = ( std::uint32_t )pbi->PebBaseAddress + offsetof(PEB64, NtGlobalFlag);

	return m_processManagement32.getVmm().putVar(currFlag, flagAddress);
}

bool AntiDbg::bypassAll() {
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



// NtGlobalFlag field exists at offset 0x68 in the
// Process Environment Block on the 32 - bit versions of
// Windows, and at offset 0xBC on the 64-bit versions of
// Windows.

