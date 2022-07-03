#include "BeingDebugged32.h"

BeingDebugged32::BeingDebugged32(ProcessManagement32& processManagement) : BaseBypass32(processManagement) {}

bool BeingDebugged32::bypass() {
	if(isBeingDebugged32()) {
		if(!bypassBeingDebugged32()) {
			return false;
		}
	}
	return true;
}

bool BeingDebugged32::isBeingDebugged32() {
	auto peb = m_processManagement.getStructures().getPEB32();
	if(!peb) {
		return false;
	}

	return peb->BeingDebugged > 0;
}


bool BeingDebugged32::bypassBeingDebugged32() {
	auto peb = m_processManagement.getStructures().getPEB32();
	auto pebVa = m_processManagement.getStructures().getPEB32Va();
	if(!peb || !pebVa) {
		return false;
	}

	auto beingDebuggedVa = *pebVa + offsetof(PEB32, BeingDebugged);
	BYTE zeroStatus = 0;

	return m_processManagement.getVmm().putVar(zeroStatus, beingDebuggedVa);
}