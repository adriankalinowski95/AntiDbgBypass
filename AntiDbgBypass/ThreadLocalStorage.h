#pragma once
#include "BaseBypass32.h"

class ThreadLocalStorage : public BaseBypass32 {
public:
	ThreadLocalStorage(ProcessManagement32& processManegement32);
	bool bypass();
};

