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

std::optional<std::uint32_t> ProcessManagementUM::allocMemory(std::uint32_t size, std::uint32_t protect) {
	if(!m_processHandle) {
		return std::nullopt;
	}

	return (std::uint32_t)VirtualAllocEx(*m_processHandle, NULL, size, MEM_COMMIT | MEM_RESERVE, protect);
}

bool ProcessManagementUM::freeMemory(std::uint32_t address) {
	if(!m_processHandle) {
		return false;
	}

	return VirtualFreeEx(*m_processHandle, (LPVOID)address, 0, MEM_RELEASE);
}

bool ProcessManagementUM::protectMemory(uint32_t address, uint32_t dwSize, uint32_t flNewProtect, uint32_t* lpflOldProtect) {
	return VirtualProtectEx(*m_processHandle, (LPVOID)address, dwSize, flNewProtect, (PDWORD)lpflOldProtect);
}
