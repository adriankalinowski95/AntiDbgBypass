#pragma once
#include "BaseBypass32.h"

class Heap32 : BaseBypass32 {
public:
	Heap32(ProcessManagement32& processManagement);
	bool bypass();

private:
	bool isGlobalFlagsClearInProcess();
};

	