#pragma once
#include "ProcessManagement32.h"

class BaseBypass32 {
public:
	BaseBypass32(ProcessManagement32& processManagement);
	virtual bool bypass();

protected:
	ProcessManagement32& m_processManagement;
};