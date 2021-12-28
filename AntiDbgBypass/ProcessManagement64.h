#pragma once
#include "ProcessManagement.h"

class ProcessManagement64 : ProcessManagement<std::uint64_t> {
public:
	ProcessManagement64(std::string processName);

private:
	std::string processName;

};

