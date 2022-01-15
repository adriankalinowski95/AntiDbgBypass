#pragma once

#include "ProcessManagement.h"

class ProcessManagementUM : public ProcessManagement<std::uint32_t> {
public:
	ProcessManagementUM(std::string processName);
	~ProcessManagementUM();
	std::optional<std::int32_t> getModuleInformation(get_module_information& mod) override;
	std::optional<std::int32_t> readMemory(std::uint32_t address, std::uint8_t* buffer, std::uint32_t size) override;
	std::optional<std::int32_t> writeMemory(std::uint32_t address, std::uint8_t* buffer, std::uint32_t size) override;
	std::optional<std::uint32_t> allocMemory(std::uint32_t size, std::uint32_t protect) override;
	bool freeMemory(std::uint32_t address) override;
	bool protectMemory(uint32_t address, uint32_t dwSize, uint32_t flNewProtect, uint32_t* lpflOldProtect) override;

private:
	std::string m_processName;
	void closeHandle();
};

