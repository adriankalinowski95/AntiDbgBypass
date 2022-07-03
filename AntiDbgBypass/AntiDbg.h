#pragma once
#include "ProcessManagement32.h"
#include "Vmm32UM.h"
#include "LoaderUM.h"
#include "ProcessStructures32UM.h"
#include "ProcessStructures32Wow64UM.h"

class AntiDbg {
public:
	AntiDbg(std::string processName);
	bool bypassAll();

private:
	std::string m_processName;
	LoaderUM m_loaderUM;
	Vmm32UM m_vmm32UM;
	ProcessStructures32UM m_processStructures32UM;
	ProcessStructures32Wow64UM m_processStructures32Wow64UM;

	ProcessManagement32 m_processManagement32;
	ProcessManagement32 m_processManagement32Wow64;
};

