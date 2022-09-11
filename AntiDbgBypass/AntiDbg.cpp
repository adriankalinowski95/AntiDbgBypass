#include "AntiDbg.h"
#include "NtGlobalFlag32.h"
#include "GlobalFlagsClear32.h"
#include "BeingDebugged32.h"
#include "ProcessHeapFlags32.h"
#include "CheckRemoteDebuggerPresent32.h"
#include "NtQueryInformationProcess32.h"
#include "Heap32.h"

#include <cstddef>

AntiDbg::AntiDbg(std::string processName):
	m_processName{ processName },
	m_loaderUM{ processName },
	m_vmm32UM{ m_loaderUM },
	m_processStructures32UM{ m_vmm32UM },
	m_processStructures32Wow64UM{ m_vmm32UM },
	m_processManagement32{ m_vmm32UM, m_processStructures32UM },
	m_processManagement32Wow64{ m_vmm32UM, m_processStructures32Wow64UM }{}

bool AntiDbg::bypassAll() {
	auto isNtGlobalFlag32Bypass = NtGlobalFlag32(m_processManagement32).bypass();
	auto isNtGlobalFlag32Wow64Bypass = NtGlobalFlag32(m_processManagement32Wow64).bypass();
	
	auto beingDebugged32Bypass = BeingDebugged32(m_processManagement32).bypass();
	auto beingDebugged32Wow64Bypass = BeingDebugged32(m_processManagement32Wow64).bypass();

	auto globalFlagClear32Bypass = GlobalFlagsClear32(m_processManagement32).bypass();

	auto processHeap32Bypass = ProcessHeapFlags32(m_processManagement32).bypass();
	auto processHeap32Wow64Bypass = ProcessHeapFlags32(m_processManagement32Wow64).bypass();

	auto ntQueryInformationProcess32Bypass = NtQueryInformationProcess32(m_processManagement32).bypass();

	auto heap32Bypass = Heap32(m_processManagement32).bypass();
	auto heap32Wow64Bypass = Heap32(m_processManagement32Wow64).bypass();

	return true;
}



// NtGlobalFlag field exists at offset 0x68 in the
// Process Environment Block on the 32 - bit versions of
// Windows, and at offset 0xBC on the 64-bit versions of
// Windows.

