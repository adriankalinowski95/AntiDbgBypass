#include "ProcessManagement32.h"
#include <Windows.h>
#include "psapi.h"

ProcessManagement32::ProcessManagement32(ProcessManagement<std::uint32_t>& processManagment):
					m_processManagement{ processManagment },
					m_vmm{ m_processManagement} {}

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

std::optional<PROCESS_BASIC_INFORMATION> ProcessManagement32::getPBI() {
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

	pbi.PebBaseAddress = (PPEB)( (ULONG_PTR)pbi.PebBaseAddress + Page_Size );

	return pbi;
}

std::optional<PROCESS_BASIC_INFORMATION> ProcessManagement32::getPBIWow64() {
	BOOL isWow64 = FALSE;
	if(!IsWow64Process(m_processManagement.getProcessHandle().value(), &isWow64)) {
		std::nullopt;
	}

	if(!isWow64) {
		return std::nullopt;
	}

	PROCESS_BASIC_INFORMATION pbi{};
	if (NtQueryInformationProcess(m_processManagement.getProcessHandle().value(), ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION_WOW64), NULL) != 0) {
		return std::nullopt;
	}

	return PROCESS_BASIC_INFORMATION{};
}

std::optional<PEB32> ProcessManagement32::getPEB() {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	auto pbi = getPBI();
	if(!pbi) {
		return std::nullopt;
	}

	PEB32 peb{};
	const auto readedMemory = m_processManagement.readMemory(reinterpret_cast<std::uint32_t>( pbi->PebBaseAddress ),
															 reinterpret_cast<std::uint8_t*>(&peb), sizeof(peb));
	if(!readedMemory || readedMemory != sizeof(peb)) {
		return std::nullopt;
	}

	return peb;
}

std::optional<PEB32> ProcessManagement32::getPEBWow64() {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	auto pbi = getPBIWow64();
	if(!pbi) {
		return std::nullopt;
	}

	PEB32 peb{};
	const auto readedMemory = m_processManagement.readMemory(reinterpret_cast<std::uint32_t>( pbi->PebBaseAddress ),
															 reinterpret_cast<std::uint8_t*>( &peb ), sizeof(peb));
	if(!readedMemory || readedMemory != sizeof(peb)) {
		return std::nullopt;
	}

	return peb;
}

std::optional<std::uint32_t> ProcessManagement32::getPEBAddress() {
	return std::optional<std::uint32_t>();
}

Vmm32& ProcessManagement32::getVmm() {
	return m_vmm;
}
