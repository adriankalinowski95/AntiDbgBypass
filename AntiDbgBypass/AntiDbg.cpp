#include "AntiDbg.h"

AntiDbg::AntiDbg(std::string processName):
	m_processManagement32{ processName} {}

bool AntiDbg::isNtGlobalFlag32On() {
	auto peb = m_processManagement32.getPEB();
	if(!peb) {
		return false;
	}

	if(peb->NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED) {
		return true;
	}

	return false;
}

bool AntiDbg::bypassNtGlobalFlagCheck() {
	
	return false;
}



// NtGlobalFlag field exists at offset 0x68 in the
// Process Environment Block on the 32 - bit versions of
// Windows, and at offset 0xBC on the 64-bit versions of
// Windows.

