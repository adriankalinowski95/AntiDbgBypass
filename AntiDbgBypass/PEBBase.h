#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include "PEBAbstraction.h"

template <class T>
class PEBBase : public PEBAbstraction {
public:
	PEBBase(T peb, std::uint64_t va):
		m_peb{ peb },
		m_va{ va } {}

	size_t getSize() override {
		return sizeof(T::ImageBaseAddress);
	}

	std::uint64_t getVa() override  {
		return m_va;
	}

	BYTE getBeingDebugged() override  {
		return m_peb.BeingDebugged;
	}

	size_t getBeingDebuggedOffset() override  {
		return offsetof(T, BeingDebugged);
	}

	DWORD getNtGlobalFlag() override {
		return m_peb.NtGlobalFlag;
	}

	size_t getNtGlobalFlagOffset() override {
		return offsetof(T, NtGlobalFlag);
	}

	std::uint64_t getProcessHeap() override {
		return m_peb.ProcessHeap;
	}

	size_t getProcessHeapOffset() override {
		return offsetof(T, ProcessHeap);
	}

	std::uint64_t getProcessHeaps() override {
		return m_peb.ProcessHeaps;
	}

	size_t getProcessHeapsOffset() override {
		return offsetof(T, ProcessHeaps);
	}

	DWORD getNumberOfHeaps() override {
		return m_peb.NumberOfHeaps;
	}

private:
	T m_peb;
	std::uint64_t m_va;
};

