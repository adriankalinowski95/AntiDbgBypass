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

private:
	ProcessManagement<std::uint32_t>& m_processManagement;
	Vmm32 m_vmm;
};	