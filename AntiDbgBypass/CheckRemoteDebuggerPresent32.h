#pragma once
#include "BaseBypass32.h"

class CheckRemoteDebuggerPresent32 : BaseBypass32 {
public:
	CheckRemoteDebuggerPresent32(ProcessManagement32& processManegement32);
	bool bypass();
};

