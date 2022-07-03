#include "Hook32.h"
#include "ShellcodeGenerator.h"

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

bool Hook32::overwriteFunction(std::string dllName, std::string funName, std::vector<std::uint8_t>& newFun, std::uint8_t paramsCount) {
	auto moduleHandle = m_processManagement32.getStructures().getRemoteModuleHandle(dllName.c_str());
	if(!moduleHandle) {
		return false;
	}

	auto procVa = m_processManagement32.getStructures().getRemoteProcAddress(*moduleHandle, funName.c_str(), 0, 0);
	if(!procVa) {
		return false;
	}

	auto hookFunctionVa = m_processManagement32.injectData(newFun);
	if(!hookFunctionVa) {
		return false;
	}

	return createHook(( std::uint32_t )*procVa, *hookFunctionVa, paramsCount);
}