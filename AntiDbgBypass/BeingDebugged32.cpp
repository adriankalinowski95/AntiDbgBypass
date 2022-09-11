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
	auto peb = m_processManagement.getStructures().getPEB();
	if(!peb) {
		return false;
	}

	return peb->getBeingDebugged() > 0;
}


bool BeingDebugged32::bypassBeingDebugged32() {
	auto peb = m_processManagement.getStructures().getPEB();
	auto pebVa = m_processManagement.getStructures().getPEBVa();
	if(!peb || !pebVa) {
		return false;
	}

	auto beingDebuggedVa = *pebVa + peb->getBeingDebuggedOffset();
	BYTE zeroStatus = 0;

	return m_processManagement.getVmm().putVar(zeroStatus, beingDebuggedVa);
}