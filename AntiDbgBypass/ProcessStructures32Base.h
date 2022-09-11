#pragma once
#include "VmmBase.h"
#include <optional>
#include "peb.h"
#include "ntddk.h"
#include <vector>
#include "ntddk.h"
#include "PEB32C.h"

class ProcessStructures32Base {
public:
	using ImgLoadConfDir32_V = std::pair<std::uint32_t, IMAGE_LOAD_CONFIG_DIRECTORY32>;
	using HeapBlock32 = std::pair<std::uint32_t, HEAP_UNPACKED_PACKET_32>;

	ProcessStructures32Base(VmmBase<std::uint32_t>& vmm) : 
		m_vmm{ vmm } {}
	
	std::optional<IMAGE_NT_HEADERS32> getNtHeader();
	std::optional<PROCESS_BASIC_INFORMATION> getPBI();
	std::optional<THREAD_BASIC_INFORMATION> getTBI();
	std::optional<ImgLoadConfDir32_V> getImageLoadConfigDirectory();
	std::optional<HMODULE> getRemoteModuleHandle(LPCSTR lpModuleName);
	std::optional<FARPROC> getRemoteProcAddress(HMODULE hModule, LPCSTR lpProcName, UINT Ordinal, BOOL UseOrdinal);
	std::vector<HEAP32> getHeaps();
	std::vector<HeapBlock32> getHeapBlocks(HEAP32& heap);
	
	// Change to std::shared_ptr
	// Main function who is getting PEB
	PEBAbstraction* getPEB();
	
	// We have 2 options to get PEB - from TEB and TBI.
	virtual std::optional<std::uint64_t> getPEBVa() = 0;

	// virtual abstraction, who is using a getPEBVa, used in getPEB
	virtual PEBAbstraction* getPEBByVa(std::uint64_t va) = 0;

protected:
	VmmBase<std::uint32_t>& m_vmm;
	// PEBAbstraction* basePEB;

	static constexpr std::uint32_t Max_Heaps_Count = 128;
	static constexpr std::uint32_t Max_Heap_Blocks_Count = 128;
	static constexpr std::uint8_t Heap_Block_Granulation = 8;
};

