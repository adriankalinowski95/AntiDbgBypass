#pragma once
#include "ProcessManagement.h"
#include "ProcessManagementUtils.h"
#include "Vmm32.h"
#include "peb.h"
#include <optional>
#include <vector>

class ProcessManagement32 {
public:
	using ImgLoadConfDir32_V = std::pair<std::uint32_t, IMAGE_LOAD_CONFIG_DIRECTORY32>;
	using HeapBlock32 = std::pair<std::uint32_t, HEAP_UNPACKED_PACKET_32>;

	ProcessManagement32(ProcessManagement<std::uint32_t>& processManagment);
	~ProcessManagement32();
	Vmm32& getVmm();
	std::optional<IMAGE_NT_HEADERS32> getNtHeader();
	std::optional<PROCESS_BASIC_INFORMATION> getPBI();
	std::uint32_t getPEB32FromPBI(PROCESS_BASIC_INFORMATION& pbi);
	std::optional<PEB32> getPEB32();
	std::optional<PEB64> getPEB64();
	std::optional<ImgLoadConfDir32_V> getImageLoadConfigDirectory();
	std::optional<HMODULE> getRemoteModuleHandle(LPCSTR lpModuleName);
	std::optional<FARPROC> getRemoteProcAddress(HMODULE hModule, LPCSTR lpProcName, UINT Ordinal, BOOL UseOrdinal);
	std::optional<std::uint32_t> injectData(std::vector<std::uint8_t>& data);
	bool freeMemory(std::uint32_t address);
	std::vector<HEAP32> getHeaps();
	std::vector<HeapBlock32> getHeapBlocks(HEAP32& heap);
	std::vector<std::pair<std::uint32_t, std::uint32_t>> getBasicBlocks();

private:
	ProcessManagement<std::uint32_t>& m_processManagement;
	Vmm32 m_vmm;

	static constexpr std::uint32_t Max_Heaps_Count = 128;
	static constexpr std::uint32_t Max_Heap_Blocks_Count = 128;
	static constexpr std::uint8_t Heap_Block_Granulation = 8;
};	