#include "ProcessManagement32.h"
#include <Windows.h>


ProcessManagement32::ProcessManagement32(std::string processName):
					 m_processName(processName) {}

std::optional<std::int32_t> ProcessManagement32::getModuleInformation(get_module_information& mod) {
	auto ntHeader = getNtHeader();
	if(!ntHeader) {
		return std::nullopt;
	}

	mod.imageBase = ntHeader->OptionalHeader.ImageBase;
	mod.sizeOfImage = ntHeader->OptionalHeader.SizeOfImage;

	return true;
}

std::optional<std::int32_t> ProcessManagement32::readMemory(std::uint32_t address, std::uint32_t* buffer, std::uint32_t size) {
	const auto processHandle = ProcessManagementUtils::getProcessHandleByName(m_processName);
	if (!processHandle) {
		return std::nullopt;
	}

	SIZE_T readedBytes{};
	if(!ReadProcessMemory(*processHandle, (LPCVOID)address, (LPVOID)buffer, size, &readedBytes)) {
		CloseHandle(*processHandle);

		return std::nullopt;
	}

	CloseHandle(*processHandle);

	return (std::int32_t)readedBytes;
}

std::optional<std::int32_t> ProcessManagement32::writeMemory(std::uint32_t address, std::uint32_t* buffer, std::uint32_t size) {
	const auto processHandle = ProcessManagementUtils::getProcessHandleByName(m_processName);
	if(!processHandle) {
		return std::nullopt;
	}

	SIZE_T writedBytes{};
	if(!WriteProcessMemory(*processHandle, (LPVOID)address, (LPVOID)buffer, size, &writedBytes)) {
		CloseHandle(*processHandle);

		return std::nullopt;
	}

	return (std::int32_t)writedBytes;
}

std::optional<std::uint64_t> ProcessManagement32::getProcessBaseAddress() {
	auto processHandle = ProcessManagementUtils::getProcessHandleByName(m_processName);
	if(!processHandle) {
		return std::nullopt;
	}

	return ProcessManagementUtils::getProcessBaseAddress(*processHandle);
}

std::optional<IMAGE_NT_HEADERS> ProcessManagement32::getNtHeader() {
	auto processHandle = ProcessManagementUtils::getProcessHandleByName(m_processName);
	if(!processHandle) {
		return std::nullopt;
	}

	auto baseAddress = ProcessManagementUtils::getProcessBaseAddress(*processHandle);
	if(!baseAddress) {
		return std::nullopt;
	}

	std::uint8_t headersBuf[Page_Size];
	std::uint32_t readBytes;
	if(!ReadProcessMemory(*processHandle, (LPCVOID)*baseAddress, headersBuf, sizeof(headersBuf), (SIZE_T)&readBytes) || readBytes != sizeof(headersBuf)) {
		return std::nullopt;
	}

	return ProcessManagementUtils::getNtHeaders32(headersBuf);
}