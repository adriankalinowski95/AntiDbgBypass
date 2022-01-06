#include "AntiDbg.h"
#include "NtGlobalFlag32.h"
#include "GlobalFlagsClear32.h"
#include "BeingDebugged32.h"
#include "ProcessHeapFlags32.h"

#include <cstddef>

AntiDbg::AntiDbg(std::string processName):
	m_rocessManagementUM{ processName },
	m_processManagement32{ m_rocessManagementUM } {
	}

bool AntiDbg::bypassAll() {
	auto isNtGlobalFlag32Bypass = NtGlobalFlag32(m_processManagement32).bypass();
	auto beingDebugged32Bypass = BeingDebugged32(m_processManagement32).bypass();
	auto globalFlagClear32Bypass = GlobalFlagsClear32(m_processManagement32).bypass();
	auto processHeap32Bypass = ProcessHeapFlags32(m_processManagement32).bypass();

	return true;
}



// NtGlobalFlag field exists at offset 0x68 in the
// Process Environment Block on the 32 - bit versions of
// Windows, and at offset 0xBC on the 64-bit versions of
// Windows.

