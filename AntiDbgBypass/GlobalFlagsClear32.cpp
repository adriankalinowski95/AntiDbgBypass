#include "GlobalFlagsClear32.h"

GlobalFlagsClear32::GlobalFlagsClear32(ProcessManagement32& processManagement): BaseBypass32(processManagement) {}

bool GlobalFlagsClear32::bypass() {
	if(isGlobalFlagsClearInProcess()) {
		if(!bypassGlobalFlagsClearInProcess()) {
			return false;
		}
	}

	return true;
}

bool GlobalFlagsClear32::isGlobalFlagsClearInProcess() {
	auto loadConfigDirectory = m_processManagement.getStructures().getImageLoadConfigDirectory();
	if(!loadConfigDirectory) {
		return false;
	}

	if(loadConfigDirectory->second.GlobalFlagsClear != 0) {
		return true;
	}

	return false;
}

bool GlobalFlagsClear32::bypassGlobalFlagsClearInProcess() {
	auto loadConfigDirectory = m_processManagement.getStructures().getImageLoadConfigDirectory();
	if(!loadConfigDirectory) {
		return false;
	}
	
	auto globalFlagsClearVa = loadConfigDirectory->first + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, GlobalFlagsClear);
	DWORD newGlobalFlagsClear = 0;

	return m_processManagement.getVmm().putVar(newGlobalFlagsClear, globalFlagsClearVa);
}
