#pragma once
#include "VmmBase.h"

template<typename T>
class ProcessManagement {
public:
	ProcessManagement(VmmBase<T>& vmmBase) : m_vmm{ vmmBase } {}
	
	auto& getVmm() {
		return m_vmm;
	}

protected:
	VmmBase<T>& m_vmm;
};
