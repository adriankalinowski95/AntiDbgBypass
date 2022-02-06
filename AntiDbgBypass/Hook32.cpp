#include "Hook32.h"
#include "ShellcodeGenerator.h"


std::vector<uint8_t> shellcode2 = {
0x55 ,0x8B ,0xEC ,0x81,
 0xEC ,0xC0 ,0x00 ,0x00,
 0x00 ,0x53 ,0x56 ,0x57,
 0x8D ,0xBD ,0x40 ,0xFF,
 0xFF ,0xFF ,0xB9 ,0x30,
 0x00 ,0x00 ,0x00 ,0xB8,
 0xCC ,0xCC ,0xCC ,0xCC,
 0xF3 ,0xAB ,0xB9 ,0x35,
 0x10 ,0x45 ,0x00,
 0xB8 ,0x01 ,0x00 ,0x00,
 0x00 ,0x5F ,0x5E ,0x5B,
 0x81 ,0xC4 ,0xC0 ,0x00,
 0x00 ,0x00 ,0x3B ,0xEC,
 0x8B ,0xE5 ,0x5D,
 0xC2 ,0x04 ,0x00
};

std::vector<uint8_t> NtQueryInformationProcessShellcode = { 0x55,0x8b,0xec,0x8b,0x45,0x0c,0x83,0x38,0x07,0x74,
0x10,0x8b,0x4d,0x0c,0x83,0x39,0x1f,0x74,0x08,0x8b,
0x55,0x0c,0x83,0x3a,0x1e,0x75,0x09,0x8b,0x45,0x0c,
0xc7,0x00,0x00,0x00,0x00,0x00,0xb8,0xef,0xee,0xee,
0xbe,0x5d,0xc2,0x14,0x00 };

std::vector<uint8_t> NtQueryInformationProcessShellcodeRet1 =
{
	0x55, 0x8b, 0xec, 0x8b, 0x45, 0x0c, 0x83, 0x38, 0x07, 0x74,
		0x10, 0x8b, 0x4d, 0x0c, 0x83, 0x39, 0x1f, 0x74, 0x08, 0x8b,
		0x55, 0x0c, 0x83, 0x3a, 0x1e, 0x75, 0x10, 0x8b, 0x45, 0x0c,
		0xc7, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x01, 0x00, 0x00,
		0x00, 0xeb, 0x05, 0xb8, 0xef, 0xee, 0xee, 0xbe, 0x5d, 0xc2,
		0x14, 0x00
};

Hook32::Hook32(ProcessManagement32& processManagement): m_processManagement32{ processManagement } {}

bool Hook32::createHook(std::uint32_t overwriteVa, std::uint32_t hookFunctionVa, std::uint8_t paramsCount) {
	auto overwritedCode = getOverwritedCode(overwriteVa, getJumpShellcodeSize());
	auto detour = getDetour(hookFunctionVa, paramsCount, overwriteVa + getJumpShellcodeSize(), overwritedCode);
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
	auto detourNearVa = ( detourVa - overwriteVa ) - getJumpShellcodeSize();
	std::uint32_t oldProtect{};
	if(!m_processManagement32.getVmm().protectMemory(overwriteVa, getJumpShellcodeSize(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
		return false;
	}

	std::vector<std::uint8_t> shellcode(getJumpShellcodeSize(), 0);
	shellcode[0] = 0xE9;
	shellcode[1] = ( detourNearVa ) & 0xFF;
	shellcode[2] = ( detourNearVa >> 8 ) & 0xFF;
	shellcode[3] = ( detourNearVa >> 16 ) & 0xFF;
	shellcode[4] = ( detourNearVa >> 24 ) & 0xFF;
	if(!m_processManagement32.getVmm().putData(shellcode.data(), shellcode.size(), overwriteVa)) {
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

std::vector<std::uint8_t> Hook32::getBranchChanges(std::uint8_t prefix, std::uint32_t functionVa, std::uint32_t overwritedVa) {
	std::vector<std::uint8_t> changeBranchInstr(getJumpShellcodeSize(),0);
	changeBranchInstr[0] = prefix;

	auto hookNearVa = functionVa - overwritedVa - getJumpShellcodeSize();
	changeBranchInstr[1] = ( hookNearVa ) & 0xFF;
	changeBranchInstr[2] = ( hookNearVa >> 8 ) & 0xFF;
	changeBranchInstr[3] = ( hookNearVa >> 16 ) & 0xFF;
	changeBranchInstr[4] = ( hookNearVa >> 24 ) & 0xFF;

	return changeBranchInstr;
}

std::vector<std::uint8_t> Hook32::getJumpOverwrite(std::uint32_t functionVa, std::uint32_t overwritedVa) {
	return getBranchChanges(0xE9, functionVa, overwritedVa);
}

std::vector<std::uint8_t> Hook32::getCmpEax(std::uint32_t value) {
	std::vector<std::uint8_t> cmp(Cmp_Shellcode_Size, 0);
	cmp[0] = 0x3D;
	cmp[1] = ( value ) & 0xFF;
	cmp[2] = ( value >> 8 ) & 0xFF;
	cmp[3] = ( value >> 16 ) & 0xFF;
	cmp[4] = ( value >> 24 ) & 0xFF;

	return cmp;
}

std::vector<std::uint8_t> Hook32::getMovEax(std::uint32_t value) {
	std::vector<std::uint8_t> movEax(Mov_Eax_Shellcode_Size, 0);
	movEax[0] = 0xB8;
	movEax[1] = ( value ) & 0xFF;
	movEax[2] = ( value >> 8 ) & 0xFF;
	movEax[3] = ( value >> 16 ) & 0xFF;
	movEax[4] = ( value >> 24 ) & 0xFF;

	return movEax;
}

std::vector<std::uint8_t> Hook32::getCallOverwrite(std::uint32_t functionVa, std::uint32_t overwritedVa) {
	return getBranchChanges(0xE8, functionVa, overwritedVa);
}

std::vector<std::uint8_t> Hook32::getJEShellcode(std::uint32_t labelVa, std::uint32_t overwritedVa) {
	std::vector<std::uint8_t> changeBranchInstr(Je_Shellcode_Size, 0);
	changeBranchInstr[0] = 0xf;
	changeBranchInstr[1] = 0x84;

	auto hookNearVa = labelVa - overwritedVa - Je_Shellcode_Size;
	changeBranchInstr[2] = ( hookNearVa ) & 0xFF;
	changeBranchInstr[3] = ( hookNearVa >> 8 ) & 0xFF;
	changeBranchInstr[4] = ( hookNearVa >> 16 ) & 0xFF;
	changeBranchInstr[5] = ( hookNearVa >> 24 ) & 0xFF;

	return changeBranchInstr;
}

std::vector<std::uint8_t> Hook32::getJNEShellcode(std::uint32_t labelVa, std::uint32_t overwritedVa) {
	std::vector<std::uint8_t> changeBranchInstr(Je_Shellcode_Size, 0);
	changeBranchInstr[0] = 0xf;
	changeBranchInstr[1] = 0x85;

	auto hookNearVa = labelVa - overwritedVa - Je_Shellcode_Size;
	changeBranchInstr[2] = ( hookNearVa ) & 0xFF;
	changeBranchInstr[3] = ( hookNearVa >> 8 ) & 0xFF;
	changeBranchInstr[4] = ( hookNearVa >> 16 ) & 0xFF;
	changeBranchInstr[5] = ( hookNearVa >> 24 ) & 0xFF;

	return changeBranchInstr;
}

std::vector<std::uint8_t> Hook32::getRetN(std::uint16_t value) {
	std::vector<std::uint8_t> retn(RetN_Shellcode_Size, 0);
	retn[0] = 0xC2;
	retn[1] = ( value ) & 0xFF;
	retn[2] = ( value >> 8 ) & 0xFF;

	return retn;
}

std::vector<std::uint8_t> Hook32::getParamsPointers(std::uint8_t paramsCount) {
	std::vector<std::uint8_t> paramsPointers;
	std::vector<std::uint8_t> leaEcxEax = { 0x8D, 0x48, 0x04 };
	std::vector<std::uint8_t> movEaxEsp = { 0x89, 0xE0 };
	std::uint8_t pushEcx = 0x51;

	paramsPointers.insert(paramsPointers.end(), movEaxEsp.begin(), movEaxEsp.end());
	for(int i = 0; i < paramsCount;i++) {
		leaEcxEax[2] = paramsCount * 0x4 - 0x4 - i * 4;
		paramsPointers.insert(paramsPointers.end(), leaEcxEax.begin(), leaEcxEax.end());
		paramsPointers.push_back(pushEcx);
	}

	return paramsPointers;
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

std::optional<Hook32::Shellcode> Hook32::getDetour(std::uint32_t hookFunctionVa, std::uint8_t paramsCount, std::uint32_t returnAddress, std::vector<uint8_t>& overwritedCode, std::uint32_t retValue) {
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

	auto newNtQueryInformationProcess = m_processManagement32.injectData(NtQueryInformationProcessShellcode);
	if(!newNtQueryInformationProcess) {
		return false;
	}

	return createHook((std::uint32_t)*procAddress, *newNtQueryInformationProcess, 5);
}