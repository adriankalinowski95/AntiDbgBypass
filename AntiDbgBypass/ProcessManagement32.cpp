#include "ProcessManagement32.h"
#include <Windows.h>
#include "psapi.h"

ProcessManagement32::ProcessManagement32(ProcessManagement<std::uint32_t>& processManagment):
					m_processManagement{ processManagment } {}

ProcessManagement32::~ProcessManagement32() {

}

std::optional<IMAGE_NT_HEADERS32> ProcessManagement32::getNtHeader() {
	if (!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	auto baseAddress = ProcessManagementUtils::getProcessBaseAddress(m_processManagement.getProcessHandle().value());
	if (!baseAddress) {
		return std::nullopt;
	}

	std::uint8_t headersBuf[Page_Size];
	auto readBytes = m_processManagement.readMemory(*baseAddress, headersBuf, sizeof(headersBuf));
	if(!readBytes) {
		return std::nullopt;
	}

	if(*readBytes != sizeof(headersBuf)) {
		return std::nullopt;
	}

	return ProcessManagementUtils::getNtHeaders32(headersBuf, *readBytes);
}

std::optional<PEB> ProcessManagement32::getPEB() {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	BOOL isWow64 = FALSE;
	if(!IsWow64Process(m_processManagement.getProcessHandle().value(), &isWow64)) {
		std::nullopt;
	}

	if(!isWow64) {
		return std::nullopt;
	}

	PROCESS_BASIC_INFORMATION pbi;
	if(NtQueryInformationProcess(m_processManagement.getProcessHandle().value(), ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL) != 0) {
		return std::nullopt;
	}

	auto processBaseAddress = ProcessManagementUtils::getProcessBaseAddress(m_processManagement.getProcessHandle().value());
	if(!processBaseAddress) {
		return std::nullopt;
	}

	auto readedMemory = m_processManagement.readMemory(( std::uint32_t )( (BYTE*)( pbi.PebBaseAddress ) + 8 ),
										( std::uint8_t* )( &( *processBaseAddress ) ), sizeof(*processBaseAddress));

	if(!readedMemory) {
		return std::nullopt;
	}

	if(readedMemory != sizeof(*processBaseAddress)) {
		return std::nullopt;
	}

	PEB peb{};
	readedMemory = m_processManagement.readMemory(( std::uint32_t )( pbi.PebBaseAddress ),( std::uint8_t* )( &peb ), sizeof(peb));
	if(!readedMemory) {
		return std::nullopt;
	}

	if(readedMemory != sizeof(peb)) {
		return std::nullopt;
	}
	return peb;
}