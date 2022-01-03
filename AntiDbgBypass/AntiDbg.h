#pragma once
#include "ProcessManagement32.h"
#include "ProcessManagementUM.h"

class AntiDbg {
public:
	AntiDbg(std::string processName);
	bool bypassAll();

private:
	ProcessManagementUM m_rocessManagementUM;
	ProcessManagement32 m_processManagement32;
};

