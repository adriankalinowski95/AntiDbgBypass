#pragma once
#include "LoaderBase.h"
#include "ProcessManagementUtils.h"

class LoaderUM : public LoaderBase {
public:
	LoaderUM(std::string processName)
		:LoaderBase(processName) 
	{
		auto handle = ProcessManagementUtils::getProcessHandleByName(processName);
		if (handle){
			setProcessHandle(*handle);
		}
	}

	~LoaderUM() {
		closeHandle();
	}

	void closeHandle() {
		if(!getProcessHandle()) {
			CloseHandle(*getProcessHandle());
			getProcessHandle().reset();
		}
	}
};