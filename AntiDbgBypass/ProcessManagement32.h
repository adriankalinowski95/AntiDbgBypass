#pragma once
#include "ProcessManagement.h"
#include "ProcessManagementUtils.h"
#include "Vmm32.h"
#include "peb.h"
#include <optional>

class ProcessManagement32 {
public:
	ProcessManagement32(ProcessManagement<std::uint32_t>& processManagment);
	~ProcessManagement32();
	Vmm32& getVmm();
	std::optional<IMAGE_NT_HEADERS32> getNtHeader();
	std::optional<PROCESS_BASIC_INFORMATION> getPBI();
	std::uint32_t getPEB32FromPBI(PROCESS_BASIC_INFORMATION& pbi);
	std::optional<PEB32> getPEB32();
	std::optional<PEB64> getPEB64();

private:
	ProcessManagement<std::uint32_t>& m_processManagement;
	Vmm32 m_vmm;
	static constexpr std::uint32_t Page_Size = 0x1000;
};	