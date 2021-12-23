#pragma once
#include "ProcessManagement.h"
#include "ProcessManagementUtils.h"
#include <optional>

class ProcessManagement32 : ProcessManagement<std::uint32_t> {
public:
	ProcessManagement32(std::string processName);
	~ProcessManagement32();
	std::optional<std::int32_t> getModuleInformation(get_module_information& mod) override;
	std::optional<std::int32_t> readMemory(std::uint32_t address, std::uint32_t* buffer, std::uint32_t size) override;
	std::optional<std::int32_t> writeMemory(std::uint32_t address, std::uint32_t* buffer, std::uint32_t size) override;
	std::optional<std::uint64_t> getProcessBaseAddress();
	std::optional<IMAGE_NT_HEADERS> getNtHeader();
	std::optional<PEB> getPEB();

private:
	std::string m_processName;
	std::optional<HANDLE> m_processHandle;

	void closeHandle();
	static constexpr std::uint32_t Page_Size = 0x1000;
};