#pragma once
#include "BaseBypass32.h"

class ProcessHeapFlags32 : BaseBypass32 {
public:
	ProcessHeapFlags32(ProcessManagement32& processManegement32);
	bool bypass();

private:
	bool isFlagsOn32(PEB32& peb);
	bool isFlagsOn64(PEB64& peb);

	bool isForceFlagsOn32(PEB32& peb);
	bool isForceFlagsOn64(PEB64& peb);

	bool bypassFlags32(PEB32& peb);
	bool bypassFlags64(PEB64& peb);
	bool bypassForceFlags32(PEB32& peb);
	bool bypassForceFlags64(PEB64& peb);

	int getHeapFlagsOffset(bool x64);
	int getHeapForceFlagsOffset(bool x64);
};

