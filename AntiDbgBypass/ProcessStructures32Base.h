#pragma once
#include "VmmBase.h"
#include <optional>
#include "peb.h"
#include "ntddk.h"
#include <vector>

class ProcessStructures32Base {
public:
	using ImgLoadConfDir32_V = std::pair<std::uint32_t, IMAGE_LOAD_CONFIG_DIRECTORY32>;
	using HeapBlock32 = std::pair<std::uint32_t, HEAP_UNPACKED_PACKET_32>;

	ProcessStructures32Base(VmmBase<std::uint32_t>& vmm) : m_vmm{vmm} {}
	virtual std::optional<PEB32> getPEB32() = 0;
	virtual std::optional<std::uint32_t> getPEB32Va() = 0;

	std::optional<IMAGE_NT_HEADERS32> getNtHeader();
	std::optional<PROCESS_BASIC_INFORMATION> getPBI();
	std::optional<ImgLoadConfDir32_V> getImageLoadConfigDirectory();
	std::optional<HMODULE> getRemoteModuleHandle(LPCSTR lpModuleName);
	std::optional<FARPROC> getRemoteProcAddress(HMODULE hModule, LPCSTR lpProcName, UINT Ordinal, BOOL UseOrdinal);
	std::vector<HEAP32> getHeaps();
	std::vector<HeapBlock32> getHeapBlocks(HEAP32& heap);

protected:
	std::optional<PEB32> getPEB32ByVa(std::uint32_t va);

	VmmBase<std::uint32_t>& m_vmm;

	static constexpr std::uint32_t Max_Heaps_Count = 128;
	static constexpr std::uint32_t Max_Heap_Blocks_Count = 128;
	static constexpr std::uint8_t Heap_Block_Granulation = 8;
};

