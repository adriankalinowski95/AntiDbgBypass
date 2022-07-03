#pragma once
#include "BaseBypass32.h"

class ProcessHeapFlags32 : BaseBypass32 {
public:
	ProcessHeapFlags32(ProcessManagement32& processManegement32);
	bool bypass();

private:
	bool isFlagsOn32(PEB32& peb);
	bool isForceFlagsOn32(PEB32& peb);
	bool bypassFlags32(PEB32& peb);
	bool bypassForceFlags32(PEB32& peb);
	int getHeapFlagsOffset(bool x64);
	int getHeapForceFlagsOffset(bool x64);
};

