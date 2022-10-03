#pragma once
#include "VmmBase.h"
#include "LoaderBase.h"
#include <optional>
#include "peb.h"
#include "ntddk.h"
#include <vector>
#include "ntddk.h"
#include "PEB32C.h"
#include <Psapi.h>
#include "IModuleEntry.h"

class ProcessStructures32Base {
public:
	using ImgLoadConfDir32_V = std::pair<std::uint32_t, IMAGE_LOAD_CONFIG_DIRECTORY32>;
	using HeapBlock32 = std::pair<std::uint32_t, HEAP_UNPACKED_PACKET_32>;

	ProcessStructures32Base(VmmBase<std::uint32_t>& vmm, LoaderBase& loaderBase) : 
		m_vmm{ vmm },
		m_loaderBase{ loaderBase } {}
	
	std::optional<IMAGE_NT_HEADERS32> getNtHeader();
	std::optional<PROCESS_BASIC_INFORMATION> getPBI();
	std::optional<THREAD_BASIC_INFORMATION> getTBI();
	std::optional<ImgLoadConfDir32_V> getImageLoadConfigDirectory();
	std::optional<HMODULE> getRemoteModuleHandle(LPCSTR lpModuleName);
	std::optional<FARPROC> getRemoteProcAddress(HMODULE hModule, LPCSTR lpProcName, UINT Ordinal, BOOL UseOrdinal);
	std::vector<HEAP32> getHeaps();
	std::vector<HeapBlock32> getHeapBlocks(HEAP32& heap);
	std::optional<MODULEINFO> getModuleInformation();

	// Change to std::shared_ptr
	// Main function who is getting PEB
	IPEB* getPEB();
	
	// We have 2 options to get PEB - from TEB and TBI.
	virtual std::optional<std::uint64_t> getPEBVa() = 0;

	// virtual abstraction, who is using a getPEBVa, used in getPEB
	virtual IPEB* getPEBByVa(std::uint64_t va) = 0;
	 
	virtual IModuleEntry* getModuleEntry();

protected:
	VmmBase<std::uint32_t>& m_vmm;
	LoaderBase& m_loaderBase;

	static constexpr std::uint32_t Max_Heaps_Count = 128;
	static constexpr std::uint32_t Max_Heap_Blocks_Count = 128;
	static constexpr std::uint8_t Heap_Block_Granulation = 8;
};

