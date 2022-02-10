#include "Hook32.h"
#include "ShellcodeGenerator.h"

std::vector<uint8_t> NtQueryInformationProcessShellcode = { 0x55,0x8b,0xec,0x8b,0x45,0x0c,0x83,0x38,0x07,0x74,
0x10,0x8b,0x4d,0x0c,0x83,0x39,0x1f,0x74,0x08,0x8b,
0x55,0x0c,0x83,0x3a,0x1e,0x75,0x09,0x8b,0x45,0x0c,
0xc7,0x00,0x00,0x00,0x00,0x00,0xb8,0xef,0xee,0xee,
0xbe,0x5d,0xc2,0x14,0x00 };


/*
std::uint32_t WINAPI overwritedNtQueryInformationProcess(IN HANDLE** ProcessHandle,
												ULONG* ProcessInformationClass,
												PVOID* ProcessInformation,
												ULONG* ProcessInformationLength,
												PULONG* ReturnLength) {
	if(*ProcessInformationClass == 0x7 || *ProcessInformationClass == 0x1f) {
		*ProcessInformationClass = 0;
		return 0xBEEEEEEF;
	}

	if(*ProcessInformationClass == 0x1e && *ReturnLength == NULL) {
		auto handlePtr = (HANDLE*)(*ProcessInformation);
		if(handlePtr != NULL) {
			*handlePtr = (void*)NULL;
		}

		return 0xC0000353L;
	}

	if(*ProcessInformationClass == 0x1e && *ReturnLength != NULL) {
		auto handlePtr = (HANDLE*)(*ProcessInformation);
		if(handlePtr != NULL) {
			*handlePtr = (void*)0x4;
		}

		*ProcessInformationLength = 0x4;

		return 0xC0000353L;
	}

	return 0xBEEEEEEF;
}
*/
std::vector<uint8_t> NtQueryInformationProcessShellcodeWithRet =
{ 0x55,0x8b,0xec,0x83,0xec,0x08,0x8b,0x45,0x0c,0x83,
0x38,0x07,0x74,0x08,0x8b,0x4d,0x0c,0x83,0x39,0x1f,
0x75,0x10,0x8b,0x55,0x0c,0xc7,0x02,0x00,0x00,0x00,
0x00,0xb8,0xef,0xee,0xee,0xbe,0xeb,0x6a,0x8b,0x45,
0x0c,0x83,0x38,0x1e,0x75,0x26,0x8b,0x4d,0x18,0x83,
0x39,0x00,0x75,0x1e,0x8b,0x55,0x10,0x8b,0x02,0x89,
0x45,0xfc,0x83,0x7d,0xfc,0x00,0x74,0x09,0x8b,0x4d,
0xfc,0xc7,0x01,0x00,0x00,0x00,0x00,0xb8,0x53,0x03,
0x00,0xc0,0xeb,0x3c,0x8b,0x55,0x0c,0x83,0x3a,0x1e,
0x75,0x2f,0x8b,0x45,0x18,0x83,0x38,0x00,0x74,0x27,
0x8b,0x4d,0x10,0x8b,0x11,0x89,0x55,0xf8,0x83,0x7d,
0xf8,0x00,0x74,0x09,0x8b,0x45,0xf8,0xc7,0x00,0x04,
0x00,0x00,0x00,0x8b,0x4d,0x14,0xc7,0x01,0x04,0x00,
0x00,0x00,0xb8,0x53,0x03,0x00,0xc0,0xeb,0x05,0xb8,
0xef,0xee,0xee,0xbe,0x8b,0xe5,0x5d,0xc2,0x14,0x00 };

Hook32::Hook32(ProcessManagement32& processManagement): m_processManagement32{ processManagement } {}

bool Hook32::createHook(std::uint32_t overwriteVa, std::uint32_t hookFunctionVa, std::uint8_t paramsCount) {
	auto overwritedCode = getOverwritedCode(overwriteVa, getJumpShellcodeSize());
	auto detour = getDetourWithRet(hookFunctionVa, paramsCount, overwriteVa + getJumpShellcodeSize(), overwritedCode);
	if(!detour) {
		return false;
	}
	
	if (!injectJumpShellcode(overwriteVa, detour->first)) {
		return false;
	}

	return true;
}

std::uint32_t Hook32::getJumpShellcodeSize() {
	return Jump_Shellcode_Size;
}

bool Hook32::injectJumpShellcode(std::uint32_t overwriteVa, std::uint32_t detourVa) {
	std::uint32_t oldProtect{};
	if(!m_processManagement32.getVmm().protectMemory(overwriteVa, getJumpShellcodeSize(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
		return false;
	}
	
	auto jmpToDetour = ShellcodeGenerator::getJmp(detourVa, overwriteVa);
	if(!m_processManagement32.getVmm().putData(jmpToDetour.data(), jmpToDetour.size(), overwriteVa)) {
		return false;
	}
	
	return true;
}

std::vector<std::uint8_t> Hook32::getOverwritedCode(std::uint32_t overwriteVa, std::uint32_t size) {
	std::vector<std::uint8_t> oldData(size, 0);
	if(!m_processManagement32.getVmm().getData(oldData.data(), oldData.size(), overwriteVa)) {
		return std::vector<std::uint8_t>();
	}

	return oldData;
}

std::optional<Hook32::Shellcode> Hook32::getDetour(std::uint32_t hookFunctionVa, std::uint8_t paramsCount, std::uint32_t returnAddress, std::vector<uint8_t>& overwritedCode) {
	// pop ebx
	// mov eax, esp 
	// lea ecx, [eax + offset] // paramsCount times
	// push ecx                // 
	// call address
	// overwrited code
	// push ebx
	// cmp eax, 0xBEEEEEEF
	// mov eax, 0
	// jmp ebx

	auto allocSize = 0x1000;
	auto detourVa = m_processManagement32.getVmm().allocMemory(allocSize,PAGE_EXECUTE_READWRITE); 
	if(!detourVa) {
		return std::nullopt;
	}

	ShellcodeGenerator shellcodeGenerator{ *detourVa };
	shellcodeGenerator.putPopEbx();
	shellcodeGenerator.putParamsPointetrs(paramsCount);
	shellcodeGenerator.putCall(hookFunctionVa);
	shellcodeGenerator.putPushEbx();
	shellcodeGenerator.putRawBytes(overwritedCode);
	shellcodeGenerator.putJmp(returnAddress);

	auto generatedShellcode = shellcodeGenerator.getShellcode();

	if(!m_processManagement32.getVmm().putData(generatedShellcode.data(), generatedShellcode.size(), *detourVa)) {
		return std::nullopt;
	}

	Shellcode myShellcode{};
	myShellcode.first = *detourVa;
	myShellcode.second = generatedShellcode;

	return myShellcode;
}

std::optional<Hook32::Shellcode> Hook32::getDetourWithRet(std::uint32_t hookFunctionVa, std::uint8_t paramsCount, std::uint32_t returnAddress, std::vector<uint8_t>& overwritedCode) {
	// pop ebx                      
	// push_ptr_params
	// push ecx                     
	// call 310000                  
	// push ebx                     
	// cmp eax,BEEEEEEF             
	// jne ret_from_function                   
	// mov eax,0                    
	// {before code}                  
	// jmp back_to_code
	// ret_from_function:
	// ret 14                       
	auto allocSize = 0x1000;
	auto detourVa = m_processManagement32.getVmm().allocMemory(allocSize, PAGE_EXECUTE_READWRITE);
	if(!detourVa) {
		return std::nullopt;
	}
	
	ShellcodeGenerator shellcodeGenerator{ *detourVa };
	shellcodeGenerator.putPopEbx();
	shellcodeGenerator.putParamsPointetrs(paramsCount);
	shellcodeGenerator.putCall(hookFunctionVa);
	shellcodeGenerator.putPushEbx();
	shellcodeGenerator.putCmp(Not_Return);

	auto jneAddress = shellcodeGenerator.getCurrentOffset() + Je_Shellcode_Size + Mov_Eax_Shellcode_Size + overwritedCode.size() + Jump_Shellcode_Size;
	shellcodeGenerator.putJne(jneAddress);
	shellcodeGenerator.putMovEax(0);
	shellcodeGenerator.putRawBytes(overwritedCode);
	shellcodeGenerator.putJmp(returnAddress);
	shellcodeGenerator.putRetN(paramsCount * 4);

	auto generatedShellcode = shellcodeGenerator.getShellcode();

	if(!m_processManagement32.getVmm().putData(generatedShellcode.data(), generatedShellcode.size(), *detourVa)) {
		return std::nullopt;
	}

	Shellcode myShellcode{};
	myShellcode.first = *detourVa;
	myShellcode.second = generatedShellcode;

	return myShellcode;
}

bool Hook32::overwriteNtQueryInformationProcess() {
	auto moduleHandle = m_processManagement32.getRemoteModuleHandle("ntdll.dll");
	if(!moduleHandle) {
		return false;
	}

	auto procAddress = m_processManagement32.getRemoteProcAddress(*moduleHandle, "NtQueryInformationProcess", 0, 0);
	if(!procAddress) {
		return false;
	}

	auto newNtQueryInformationProcess = m_processManagement32.injectData(NtQueryInformationProcessShellcodeWithRet);
	if(!newNtQueryInformationProcess) {
		return false;
	}

	return createHook((std::uint32_t)*procAddress, *newNtQueryInformationProcess, 5);
}