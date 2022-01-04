#pragma once
#include "BaseBypass32.h"

class ProcessHeapBypass32 : BaseBypass32 {
public:
	ProcessHeapBypass32(ProcessManagement32& processManegement32);
	bool bypass();

private:
	
};

