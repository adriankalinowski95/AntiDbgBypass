#include "ProcessManagementUtils.h"
#include <tlhelp32.h>
#include "psapi.h"

std::optional<std::uint32_t> ProcessManagementUtils::getProcessIDByName(std::string processName) {
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process{};
	process.dwSize = sizeof(process);

	if (!Process32First(snapshot, &process)) {
		return std::nullopt;
	}

	std::wstring wProcessName(processName.begin(), processName.end());
	std::uint32_t loopCounter = 1;
	std::uint32_t pid{};
	for (auto i = 0u; i < Get_Process_ID_By_Name_Loop_Counter; i++){
		std::wstring exeFileName(process.szExeFile);
		if (exeFileName.compare(wProcessName) == 0) {
			pid = process.th32ProcessID;
			break;
		}

		if (!Process32Next(snapshot, &process)) {
			CloseHandle(snapshot);

			return std::nullopt;
		}
	}
	
	CloseHandle(snapshot);

	if (!pid) { 
		return std::nullopt;
	}
	
	return pid;
}

std::optional<HANDLE> ProcessManagementUtils::getProcessHandleByName(std::string processName) {
	const auto processID = ProcessManagementUtils::getProcessIDByName(processName);
	if(!processID) {
		return std::nullopt;
	}

	const auto processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, *processID);
	if(!processHandle) {
		return std::nullopt;
	}
	
	return processHandle;
}

std::optional<std::uint64_t> ProcessManagementUtils::getProcessBaseAddress(HANDLE processHandle) {
	std::uint64_t baseAddress = 0;
	HMODULE* moduleArray = NULL;
	LPBYTE moduleArrayBytes = NULL;
	std::uint32_t bytesRequired = 0;

	if(!processHandle) {
		return std::nullopt;
	}

	if(!EnumProcessModules(processHandle, NULL, 0, (LPDWORD)(&bytesRequired))) {
		return std::nullopt;
	}

	if(!bytesRequired) {
		return std::nullopt;
	}

	moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);
	if (!moduleArrayBytes) {
		return std::nullopt;
	}

	std::uint32_t moduleCount{};
	moduleCount = bytesRequired / sizeof(HMODULE);
	moduleArray = (HMODULE*)moduleArrayBytes;
	
	if(EnumProcessModules(processHandle, moduleArray, bytesRequired, (LPDWORD)(&bytesRequired))) {
		baseAddress = (std::uint64_t)moduleArray[0];
		LocalFree(moduleArrayBytes);
		CloseHandle(processHandle);

		return baseAddress;
	}

	LocalFree(moduleArrayBytes);
	CloseHandle(processHandle);
	
	return std::nullopt;
}



std::optional<IMAGE_NT_HEADERS> ProcessManagementUtils::getNtHeaders32(BYTE* peBuffer) {
	if(!peBuffer) {
		return std::nullopt;
	}

	IMAGE_DOS_HEADER* idh = (IMAGE_DOS_HEADER*)peBuffer;
	if(idh->e_magic != IMAGE_DOS_SIGNATURE) {
		return std::nullopt;
	}

	const LONG kMaxOffset = 1024;
	LONG pe_offset = idh->e_lfanew;
	if(pe_offset > kMaxOffset){
		return std::nullopt;
	}

	IMAGE_NT_HEADERS* inh = (IMAGE_NT_HEADERS*)( (BYTE*)peBuffer + pe_offset );
	IMAGE_NT_HEADERS imageNtHeaders{};
	memcpy(&imageNtHeaders, inh, sizeof(IMAGE_NT_HEADERS));

	return imageNtHeaders;
}

std::optional<IMAGE_DATA_DIRECTORY> ProcessManagementUtils::getPEDirectory32(PVOID pe_buffer, std::uint32_t dir_id) {
	if(dir_id >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES){
		return std::nullopt;
	}

	//fetch relocation table from current image:
	auto nt_headers= getNtHeaders32((BYTE*)pe_buffer);
	if(!nt_headers){
		return std::nullopt;
	}

	IMAGE_DATA_DIRECTORY* peDir = &( nt_headers->OptionalHeader.DataDirectory[dir_id] );
	if(!peDir->VirtualAddress) {
		return std::nullopt;
	}


	IMAGE_DATA_DIRECTORY imageDataDirectory{};
	memcpy(&imageDataDirectory, peDir,sizeof(IMAGE_DATA_DIRECTORY));

	return imageDataDirectory;
}

IMAGE_OPTIONAL_HEADER ProcessManagementUtils::getOptionalHeader(IMAGE_NT_HEADERS& imageNtHeader) {
	return imageNtHeader.OptionalHeader;
}
