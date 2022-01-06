#include "ProcessHeapFlags32.h"

ProcessHeapFlags32::ProcessHeapFlags32(ProcessManagement32& processManegement32): BaseBypass32(processManegement32) {}

bool ProcessHeapFlags32::bypass() {
	auto peb32 = m_processManagement.getPEB32();
	auto peb64 = m_processManagement.getPEB64();

	if(!peb32 || !peb64) {
		return false;
	}


	if(isFlagsOn32(*peb32)) {
		if(!bypassFlags32(*peb32)) {
			return false;
		}
	}

	if(isForceFlagsOn32(*peb32)) {
		if(!bypassForceFlags32(*peb32)) {
			return false;
		}
	}

	if(isFlagsOn64(*peb64)) {
		if(!bypassFlags64(*peb64)) {
			return false;
		}
	}

	if(isForceFlagsOn64(*peb64)) {
		if(!bypassForceFlags64(*peb64)) {
			return false;
		}
	}

	return true;
} 

bool ProcessHeapFlags32::isFlagsOn32(PEB32& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapFlagVa = heapVa + getHeapFlagsOffset(false);

	std::uint32_t heapFlagValue{};
	if(!m_processManagement.getVmm().getVar(heapFlagValue, heapFlagVa)) {
		return false;
	}

	return heapFlagValue & ~HEAP_GROWABLE;
}

bool ProcessHeapFlags32::isFlagsOn64(PEB64& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapFlagVa = heapVa + getHeapFlagsOffset(true);

	std::uint64_t heapFlagValue{};
	if(!m_processManagement.getVmm().getVar(heapFlagValue, heapFlagVa)) {
		return false;
	}

	return heapFlagValue & ~HEAP_GROWABLE;
}

bool ProcessHeapFlags32::isForceFlagsOn32(PEB32& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapForceFlagVa = heapVa + getHeapForceFlagsOffset(false);

	std::uint32_t heapForceFlagValue{};
	if(!m_processManagement.getVmm().getVar(heapForceFlagValue, heapForceFlagVa)) {
		return false;
	}

	return heapForceFlagValue  != 0;
}

bool ProcessHeapFlags32::isForceFlagsOn64(PEB64& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapForceFlagVa = heapVa + getHeapForceFlagsOffset(true);

	std::uint32_t heapForceFlagValue{};
	if(!m_processManagement.getVmm().getVar(heapForceFlagValue, heapForceFlagVa)) {
		return false;
	}

	return heapForceFlagValue != 0;
}

bool ProcessHeapFlags32::bypassFlags32(PEB32& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapFlagVa = heapVa + getHeapFlagsOffset(false);

	std::uint32_t heapFlagValue{};
	return m_processManagement.getVmm().putVar(heapFlagValue, heapFlagVa);
}

bool ProcessHeapFlags32::bypassFlags64(PEB64& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapFlagVa = heapVa + getHeapFlagsOffset(true);

	std::uint64_t heapFlagValue{};
	return m_processManagement.getVmm().putVar(heapFlagValue, heapFlagVa);
}

bool ProcessHeapFlags32::bypassForceFlags32(PEB32& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapForceFlagVa = heapVa + getHeapForceFlagsOffset(false);

	std::uint32_t heapForceFlagValue{};
	return m_processManagement.getVmm().putVar(heapForceFlagValue, heapForceFlagVa);
}

bool ProcessHeapFlags32::bypassForceFlags64(PEB64& peb) {
	auto heapVa = peb.ProcessHeap;
	auto heapForceFlagVa = heapVa + getHeapForceFlagsOffset(true);

	std::uint64_t heapForceFlagValue{};
	return m_processManagement.getVmm().putVar(heapForceFlagValue, heapForceFlagVa);
}

int ProcessHeapFlags32::getHeapFlagsOffset(bool x64) {
	return x64 ?
		ProcessManagementUtils::isVistaOrHigher() ? 0x70 : 0x14 : //x64 offsets
		ProcessManagementUtils::isVistaOrHigher() ? 0x40 : 0x0C;  //x86 offsets 
}

int ProcessHeapFlags32::getHeapForceFlagsOffset(bool x64) {
	return x64 ?
		ProcessManagementUtils::isVistaOrHigher() ? 0x74 : 0x18 : //x64 offsets
		ProcessManagementUtils::isVistaOrHigher() ? 0x44 : 0x10;  //x86 offsets
}