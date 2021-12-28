#pragma once

#include "ProcessManagement.h"

class ProcessManagementUM : public ProcessManagement<std::uint32_t> {
public:
	ProcessManagementUM(std::string processName);
	~ProcessManagementUM();
	std::optional<std::int32_t> getModuleInformation(get_module_information& mod) override;
	std::optional<std::int32_t> readMemory(std::uint32_t address, std::uint8_t* buffer, std::uint32_t size) override;
	std::optional<std::int32_t> writeMemory(std::uint32_t address, std::uint8_t* buffer, std::uint32_t size) override;

private:
	std::string m_processName;
	void closeHandle();
};

