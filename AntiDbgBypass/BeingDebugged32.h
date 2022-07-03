#pragma once
#include "BaseBypass32.h"

class BeingDebugged32 : public BaseBypass32 {
public:
	BeingDebugged32(ProcessManagement32& processManagement);
	bool bypass();

private:
	bool isBeingDebugged32();
	bool bypassBeingDebugged32();
};

