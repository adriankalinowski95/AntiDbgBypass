#pragma once
#include <optional>
#include <string>
#include <Windows.h>
#include "ntddk.h"

typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

// NtQueryInformationProcess for 32-bit process on WOW64
typedef NTSTATUS(NTAPI* _NtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead );

struct PROCESS_BASIC_INFORMATION_WOW64 {
	PVOID Reserved1[2];
	PVOID64 PebBaseAddress;
	PVOID Reserved2[4];
	ULONG_PTR UniqueProcessId[2];
	PVOID Reserved3[2];
};

class ProcessManagementUtils {
public:
	static std::optional<std::uint32_t> getProcessIDByName(std::string processName);
	static std::optional<HANDLE> getProcessHandleByName(std::string processName);
	static std::optional<std::uint64_t> getProcessBaseAddress(HANDLE processHandle);
	static std::optional<IMAGE_NT_HEADERS> getNtHeaders32(BYTE* peBuffer);
	static std::optional <IMAGE_DATA_DIRECTORY> getPEDirectory32(PVOID pe_buffer, std::uint32_t dir_id);
	static IMAGE_OPTIONAL_HEADER getOptionalHeader(IMAGE_NT_HEADERS& imageNtHeader);
	static std::optional<PEB> getPEB(HANDLE processHandle);

private:
	static constexpr std::uint32_t Get_Process_ID_By_Name_Loop_Counter = 0x1000;
};
