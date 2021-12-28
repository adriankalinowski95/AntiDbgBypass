#include "ProcessManagementUM.h"
#include "ProcessManagementUtils.h"

ProcessManagementUM::ProcessManagementUM(std::string processName):
	m_processName{ processName } {
	m_processHandle = ProcessManagementUtils::getProcessHandleByName(processName);
}

ProcessManagementUM::~ProcessManagementUM() {
	closeHandle();
}

void ProcessManagementUM::closeHandle() {
	if(!m_processHandle) {
		CloseHandle(*m_processHandle);
		m_processHandle = std::nullopt;
	}
}

std::optional<std::int32_t> ProcessManagementUM::getModuleInformation(get_module_information& mod) {
	return std::nullopt;
}

std::optional<std::int32_t> ProcessManagementUM::readMemory(std::uint32_t address, std::uint8_t* buffer, std::uint32_t size) {
	if(!m_processHandle) {
		return std::nullopt;
	}

	SIZE_T readedBytes{};
	if(!ReadProcessMemory(*m_processHandle, reinterpret_cast<LPCVOID>( address ), reinterpret_cast<LPVOID>( buffer ), size, &readedBytes)) {
		return std::nullopt;
	}

	return static_cast<std::int32_t>( readedBytes );
}

std::optional<std::int32_t> ProcessManagementUM::writeMemory(std::uint32_t address, std::uint8_t* buffer, std::uint32_t size) {
	if(!m_processHandle) {
		return std::nullopt;
	}

	SIZE_T writedBytes{};
	if(!WriteProcessMemory(*m_processHandle, reinterpret_cast<LPVOID>( address ), reinterpret_cast<LPVOID>( buffer ), size, &writedBytes)) {
		return std::nullopt;
	}

	return static_cast<std::int32_t>( writedBytes );
}