#pragma once
#include "BaseBypass32.h"

class ProcessHeapFlags32 : BaseBypass32 {
public:
	ProcessHeapFlags32(ProcessManagement32& processManegement32);
	bool bypass();

private:
	bool isFlagsOn32(PEBAbstraction* peb);
	bool isForceFlagsOn32(PEBAbstraction* peb);
	bool bypassFlags32(PEBAbstraction* peb);
	bool bypassForceFlags32(PEBAbstraction* peb);
	int getHeapFlagsOffset(bool x64);
	int getHeapForceFlagsOffset(bool x64);
};

