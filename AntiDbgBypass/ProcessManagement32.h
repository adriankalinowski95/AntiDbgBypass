#pragma once
#include "ProcessManagement.h"
#include "ProcessManagementUtils.h"
#include <optional>

class ProcessManagement32 : ProcessManagement<std::uint32_t> {
public:
	ProcessManagement32(std::string processName);
	std::optional<std::int32_t> getModuleInformation(get_module_information& mod) override;
	std::optional<std::int32_t> readMemory(std::uint32_t address, std::uint32_t* buffer, std::uint32_t size) override;
	std::optional<std::int32_t> writeMemory(std::uint32_t address, std::uint32_t* buffer, std::uint32_t size) override;
	std::optional<std::uint64_t> getProcessBaseAddress();
	std::optional<IMAGE_NT_HEADERS> getNtHeader();

private:
	std::string m_processName;

	static constexpr std::uint32_t Page_Size = 0x1000;
};