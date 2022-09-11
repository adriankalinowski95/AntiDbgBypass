#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include "peb.h"

class PEBAbstraction {
public:
	virtual size_t getSize() = 0;
	virtual std::uint64_t getVa() = 0;
	virtual BYTE getBeingDebugged() = 0;
	virtual size_t getBeingDebuggedOffset() = 0;
	virtual DWORD getNtGlobalFlag() = 0;
	virtual size_t getNtGlobalFlagOffset() = 0;
	virtual std::uint64_t getProcessHeap() = 0;
	virtual size_t getProcessHeapOffset() = 0;
	virtual std::uint64_t getProcessHeaps() = 0;
	virtual size_t getProcessHeapsOffset() = 0;
	virtual DWORD getNumberOfHeaps() = 0;
};

