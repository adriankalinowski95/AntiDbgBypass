#include "ProcessStructures32Base.h"
#include "ProcessManagementUtils.h"
#include "RemoteOps.h"
#include <minwindef.h>

std::optional<IMAGE_NT_HEADERS32> ProcessStructures32Base::getNtHeader() {
	if(!m_vmm.getLoader().getProcessHandle()) {
		return std::nullopt;
	}

	auto processHandle = m_vmm.getLoader().getProcessHandle().value();
	auto baseAddress = ProcessManagementUtils::getProcessBaseAddress(processHandle);
	if(!baseAddress) {
		return std::nullopt;
	}

	std::uint8_t headersBuf[ProcessManagementUtils::Page_Size];
	auto readBytes = m_vmm.readMemory(*baseAddress, headersBuf, sizeof(headersBuf));
	if(!readBytes) {
		return std::nullopt;
	}

	if(*readBytes != sizeof(headersBuf)) {
		return std::nullopt;
	}

	return ProcessManagementUtils::getNtHeaders32(headersBuf, *readBytes);
}

std::optional<PROCESS_BASIC_INFORMATION> ProcessStructures32Base::getPBI() {
	if(!m_vmm.getLoader().getProcessHandle()) {
		return std::nullopt;
	}

	auto processHandle = m_vmm.getLoader().getProcessHandle().value();
	BOOL isWow64 = FALSE;
	if(!IsWow64Process(processHandle, &isWow64)) {
		std::nullopt;
	}

	if(!isWow64) {
		return std::nullopt;
	}

	PROCESS_BASIC_INFORMATION pbi;
	if(NtQueryInformationProcess(processHandle, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL) != 0) {
		return std::nullopt;
	}


	return pbi;
}

std::optional<PEB32> ProcessStructures32Base::getPEB32ByVa(std::uint32_t va) {
	PEB32 peb{};
	const auto readedMemory = m_vmm.getVar(peb, va);
	if(!readedMemory) {
		return std::nullopt;
	}
	
	return peb;
}

std::optional<ProcessStructures32Base::ImgLoadConfDir32_V> ProcessStructures32Base::getImageLoadConfigDirectory() {
	if(!m_vmm.getLoader().getProcessHandle()) {
		return std::nullopt;
	}

	auto processHandle = m_vmm.getLoader().getProcessHandle().value();
	auto baseAddress = ProcessManagementUtils::getProcessBaseAddress(processHandle);
	if(!baseAddress) {
		return std::nullopt;
	}

	std::uint8_t headersBuf[ProcessManagementUtils::Page_Size];
	auto readBytes = m_vmm.readMemory(*baseAddress, headersBuf, sizeof(headersBuf));
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

std::optional<HMODULE> ProcessStructures32Base::getRemoteModuleHandle(LPCSTR lpModuleName) {
	if(!m_vmm.getLoader().getProcessHandle()) {
		return std::nullopt;
	}

	auto processHandle = m_vmm.getLoader().getProcessHandle().value();

	return RemoteOps::GetRemoteModuleHandle(processHandle, lpModuleName);
}

std::optional<FARPROC> ProcessStructures32Base::getRemoteProcAddress(HMODULE hModule, LPCSTR lpProcName, UINT Ordinal, BOOL UseOrdinal) {
	if(!m_vmm.getLoader().getProcessHandle()) {
		return std::nullopt;
	}

	auto processHandle = m_vmm.getLoader().getProcessHandle().value();

	return RemoteOps::GetRemoteProcAddress(processHandle, hModule, lpProcName, Ordinal, UseOrdinal);
}

std::vector<HEAP32> ProcessStructures32Base::getHeaps() {
	std::vector<HEAP32> heaps{};
	const auto peb32 = this->getPEB32();
	if(!peb32) {
		return heaps;
	}

	const auto heapsVa = peb32->ProcessHeaps;
	const auto heapsCount = peb32->NumberOfHeaps;
	const auto alignmentHeapsCount = min(heapsCount, Max_Heaps_Count);

	for(auto i = 0u; i < Max_Heaps_Count; i++) {
		std::uint32_t heapVa{};
		if(!m_vmm.getVar(heapVa, heapsVa + sizeof(std::uint32_t) * i)) {
			return heaps;
		}

		HEAP32 heap32{};
		if(!m_vmm.getVar(heap32, heapVa)) {
			return heaps;
		}

		heaps.push_back(heap32);
	}

	return heaps;
}

std::vector<ProcessStructures32Base::HeapBlock32> ProcessStructures32Base::getHeapBlocks(HEAP32& heap) {
	std::vector<HeapBlock32> heapBlocks{};
	const auto encoding32Ptr = (std::uint32_t*)( &( heap.Encoding ) );
	const auto encoding1 = *encoding32Ptr;
	const auto encoding2 = *( encoding32Ptr + 1 );

	auto currentEntryVa = heap.FirstEntry;
	for(auto i = 0u; i < Max_Heap_Blocks_Count; i++) {
		if(currentEntryVa == heap.LastValidEntry) {
			return heapBlocks;
		}

		HEAP_UNPACKED_PACKET_32 encodedEntry{};
		if(!m_vmm.getVar(encodedEntry, currentEntryVa)) {
			return heapBlocks;
		}

		const auto encoded32Ptr = (std::uint32_t*)( &( encodedEntry ) );
		const auto encodedEntry1 = *encoded32Ptr;
		const auto encodedEntry2 = *( encoded32Ptr + 1 );

		std::uint32_t decodedEntry[2]{};
		decodedEntry[0] = encoding1 ^ encodedEntry1;
		decodedEntry[1] = encoding2 ^ encodedEntry2;

		auto decodedEntryVal = *( (HEAP_UNPACKED_PACKET_32*)( decodedEntry ) );
		decodedEntryVal.Size *= Heap_Block_Granulation;
		decodedEntryVal.PreviousSize *= Heap_Block_Granulation;

		HeapBlock32 heapBlock32{ currentEntryVa, decodedEntryVal };
		heapBlocks.push_back(heapBlock32);

		currentEntryVa += decodedEntryVal.Size;
	}

	return heapBlocks;
}