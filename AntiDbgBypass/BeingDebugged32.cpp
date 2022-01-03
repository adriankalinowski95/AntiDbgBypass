#include "BeingDebugged32.h"

BeingDebugged32::BeingDebugged32(ProcessManagement32& processManagement) : BaseBypass32(processManagement) {}

bool BeingDebugged32::bypass() {
	if(isBeingDebugged32()) {
		if(!bypassBeingDebugged32()) {
			return false;
		}
	}

	if(isBeingDebugged64()) {
		if(!bypassBeingDebugged64()) {
			return false;
		}
	}

	return true;
}

bool BeingDebugged32::isBeingDebugged32() {
	auto peb = m_processManagement.getPEB32();
	if(!peb) {
		return false;
	}

	return peb->BeingDebugged > 0;
}

bool BeingDebugged32::isBeingDebugged64() {
	auto peb = m_processManagement.getPEB64();
	if(!peb) {
		return false;
	}

	return peb->BeingDebugged > 0;
}

bool BeingDebugged32::bypassBeingDebugged32() {
	auto pbi = m_processManagement.getPBI();
	auto peb = m_processManagement.getPEB32();
	if(!peb || !pbi) {
		return false;
	}

	auto beingDebuggedVa = (std::uint32_t)m_processManagement.getPEB32FromPBI(*pbi) + offsetof(PEB32, BeingDebugged);
	BYTE zeroStatus = 0;

	return m_processManagement.getVmm().putVar(zeroStatus, beingDebuggedVa);
}

bool BeingDebugged32::bypassBeingDebugged64() {
	auto pbi = m_processManagement.getPBI();
	auto peb = m_processManagement.getPEB64();
	if(!peb || !pbi) {
		return false;
	}

	auto beingDebuggedVa = (std::uint32_t)pbi->PebBaseAddress + offsetof(PEB64, BeingDebugged);
	BYTE zeroStatus = 0;

	return m_processManagement.getVmm().putVar(zeroStatus, beingDebuggedVa);
}
