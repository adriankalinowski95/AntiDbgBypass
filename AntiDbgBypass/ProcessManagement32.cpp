#include "ProcessManagement32.h"
#include <Windows.h>
#include "psapi.h"
#include "RemoteOps.h"

ProcessManagement32::ProcessManagement32(ProcessManagement<std::uint32_t>& processManagment):
					m_processManagement{ processManagment },
					m_vmm{ m_processManagement} {}

ProcessManagement32::~ProcessManagement32() {

}

Vmm32& ProcessManagement32::getVmm() {
	return m_vmm;
}

std::optional<IMAGE_NT_HEADERS32> ProcessManagement32::getNtHeader() {
	if (!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	auto baseAddress = ProcessManagementUtils::getProcessBaseAddress(m_processManagement.getProcessHandle().value());
	if (!baseAddress) {
		return std::nullopt;
	}

	std::uint8_t headersBuf[ProcessManagementUtils::Page_Size];
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


	return pbi;
}

std::uint32_t ProcessManagement32::getPEB32FromPBI(PROCESS_BASIC_INFORMATION& pbi) {
	return reinterpret_cast<std::uint32_t>(pbi.PebBaseAddress) + ProcessManagementUtils::Page_Size;
}

std::optional<PEB32> ProcessManagement32::getPEB32() {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	auto pbi = getPBI();
	if(!pbi) {
		return std::nullopt;
	}

	PEB32 peb{};
	const auto readedMemory = m_processManagement.readMemory(getPEB32FromPBI(*pbi), reinterpret_cast<std::uint8_t*>(&peb), sizeof(peb));
	if(!readedMemory || readedMemory != sizeof(peb)) {
		return std::nullopt;
	}

	return peb;
}

std::optional<PEB64> ProcessManagement32::getPEB64() {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	auto pbi = getPBI();
	if(!pbi) {
		return std::nullopt;
	}

	PEB64 peb{};
	const auto readedMemory = m_processManagement.readMemory(reinterpret_cast<std::uint32_t>( pbi->PebBaseAddress ),
															 reinterpret_cast<std::uint8_t*>( &peb ), sizeof(peb));
	if(!readedMemory || readedMemory != sizeof(peb)) {
		return std::nullopt;
	}

	return peb;
}

std::optional<ProcessManagement32::ImgLoadConfDir32_V> ProcessManagement32::getImageLoadConfigDirectory() {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	auto baseAddress = ProcessManagementUtils::getProcessBaseAddress(m_processManagement.getProcessHandle().value());
	if(!baseAddress) {
		return std::nullopt;
	}

	std::uint8_t headersBuf[ProcessManagementUtils::Page_Size];
	auto readBytes = m_processManagement.readMemory(*baseAddress, headersBuf, sizeof(headersBuf));
	if(!readBytes) {
		return std::nullopt;
	}

	const auto loadConfig = ProcessManagementUtils::getPEDirectory32(headersBuf, ProcessManagementUtils::Page_Size, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
	if(!loadConfig) {
		return std::nullopt;
	}

	IMAGE_LOAD_CONFIG_DIRECTORY32 imageLoadConfigDirectory{};
	if(!m_vmm.getVar(imageLoadConfigDirectory, loadConfig->VirtualAddress + *baseAddress)) {
		return std::nullopt;
	}
	
	return std::make_pair(loadConfig->VirtualAddress + *baseAddress, imageLoadConfigDirectory);
}

std::optional<HMODULE> ProcessManagement32::getRemoteModuleHandle(LPCSTR lpModuleName) {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	return RemoteOps::GetRemoteModuleHandle(*m_processManagement.getProcessHandle(), lpModuleName);
}

std::optional<FARPROC> ProcessManagement32::getRemoteProcAddress(HMODULE hModule, LPCSTR lpProcName, UINT Ordinal, BOOL UseOrdinal) {
	if(!m_processManagement.getProcessHandle()) {
		return std::nullopt;
	}

	return RemoteOps::GetRemoteProcAddress(*m_processManagement.getProcessHandle(), hModule, lpProcName, Ordinal, UseOrdinal);
}

std::optional<std::uint32_t> ProcessManagement32::injectData(std::vector<std::uint8_t>& data) {
	std::uint32_t allocSize = data.size();
	if(allocSize % ProcessManagementUtils::Page_Size) {
		allocSize += ProcessManagementUtils::Page_Size - ( allocSize % ProcessManagementUtils::Page_Size );
	}

	auto injectAreaVa = m_processManagement.allocMemory(allocSize, PAGE_EXECUTE_READWRITE);
	if(!injectAreaVa) {
		return std::nullopt;
	}

	if(!m_processManagement.writeMemory(*injectAreaVa, data.data(), data.size())) {
		return false;
	}

	return injectAreaVa;
}

bool ProcessManagement32::freeMemory(std::uint32_t address) {
	return m_processManagement.freeMemory(address);
}
