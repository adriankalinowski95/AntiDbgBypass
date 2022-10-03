#pragma once
#include "BaseBypass32.h"

class ProcessHeapFlags32 : BaseBypass32 {
public:
	ProcessHeapFlags32(ProcessManagement32& processManegement32);
	bool bypass();

private:
	bool isFlagsOn32(IPEB* peb);
	bool isForceFlagsOn32(IPEB* peb);
	bool bypassFlags32(IPEB* peb);
	bool bypassForceFlags32(IPEB* peb);
	int getHeapFlagsOffset(bool x64);
	int getHeapForceFlagsOffset(bool x64);
};

