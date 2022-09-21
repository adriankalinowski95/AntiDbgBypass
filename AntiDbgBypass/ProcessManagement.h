#pragma once
#include "VmmBase.h"
#include "LoaderBase.h"

template<typename T>
class ProcessManagement {
public:
	ProcessManagement(VmmBase<T>& vmmBase,LoaderBase& loaderBase) :
		m_vmm{ vmmBase },
		m_loader{loaderBase} {}
	
	auto& getVmm() {
		return m_vmm;
	}

	auto& getLoader() {
		return m_loader;
	}

protected:
	VmmBase<T>& m_vmm;
	LoaderBase& m_loader;
};
