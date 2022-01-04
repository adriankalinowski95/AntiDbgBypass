#pragma once
#include "BaseBypass32.h"

class GlobalFlagsClear32 : public BaseBypass32 {
public:
	GlobalFlagsClear32(ProcessManagement32& processManagement);
	bool bypass();

private:
	bool isGlobalFlagsClearInProcess();
	bool bypassGlobalFlagsClearInProcess();
};

