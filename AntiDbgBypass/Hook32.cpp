#include "Hook32.h"
#include "mhook-lib/mhook.h"


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

std::optional<Hook32::Shellcode> Hook32::getDetour(std::uint32_t hookFunctionVa, std::uint8_t paramsCount, std::uint32_t returnAddress, std::vector<uint8_t>& overwritedCode) {
	// pop ebx
	// call address
	// add esp, 4 * params
	// overwrited code
	// push ebx
	// jmp ebx

	std::uint8_t popEbx = 0x5B;
	std::vector<std::uint8_t> callAddress = { 0xE8, 0xFB, 0xFF, 0xFF, 0xFF };
	std::vector<std::uint8_t> subEsp = { 0x83, 0xEC, (std::uint8_t)(4 * paramsCount)};
	std::vector<std::uint8_t> jmpBackShellcode = { 0xE9, 0xFB, 0xFF, 0xFF, 0xFF };
	std::uint8_t pushEbx = 0x53;

	auto allocSize = sizeof(popEbx) + callAddress.size() + subEsp.size() + overwritedCode.size() + jmpBackShellcode.size() + sizeof(pushEbx);
	auto detourVa = m_processManagement32.getVmm().allocMemory(allocSize,PAGE_EXECUTE_READWRITE);
	if(!detourVa) {
		return std::nullopt;
	}

	auto hookNearVa = ( hookFunctionVa - (*detourVa + 1 )) - getJumpShellcodeSize();
	callAddress[1] = ( hookNearVa ) & 0xFF;
	callAddress[2] = ( hookNearVa >> 8 ) & 0xFF;
	callAddress[3] = ( hookNearVa >> 16 ) & 0xFF;
	callAddress[4] = ( hookNearVa >> 24 ) & 0xFF;

	auto detourOffset = sizeof(popEbx) + callAddress.size() + subEsp.size() + overwritedCode.size();
	auto returnAddressNearVa = ( returnAddress - ( *detourVa + detourOffset ) ) - getJumpShellcodeSize() - 1 ;
	jmpBackShellcode[1] = ( returnAddressNearVa ) & 0xFF;
	jmpBackShellcode[2] = ( returnAddressNearVa >> 8 ) & 0xFF;
	jmpBackShellcode[3] = ( returnAddressNearVa >> 16 ) & 0xFF;
	jmpBackShellcode[4] = ( returnAddressNearVa >> 24 ) & 0xFF;


	std::vector<std::uint8_t> fullShellcode;
	fullShellcode.push_back(popEbx);
	fullShellcode.insert(fullShellcode.end(), callAddress.begin(), callAddress.end());
	fullShellcode.insert(fullShellcode.end(), subEsp.begin(), subEsp.end());
	fullShellcode.push_back(pushEbx);
	fullShellcode.insert(fullShellcode.end(), overwritedCode.begin(), overwritedCode.end());
	fullShellcode.insert(fullShellcode.end(), jmpBackShellcode.begin(), jmpBackShellcode.end());

	if(!m_processManagement32.getVmm().putData(fullShellcode.data(), fullShellcode.size(), *detourVa)) {
		return std::nullopt;
	}

	Shellcode myShellcode{};
	myShellcode.first = *detourVa;
	myShellcode.second = fullShellcode;

	return myShellcode;
}

bool Hook32::overwriteNtQueryInformationProcess() {
	auto moduleHandle = m_processManagement32.getRemoteModuleHandle("ntdll.dll");
	if(!moduleHandle) {
		return false;
	}

	auto procAddress = m_processManagement32.getRemoteProcAddress(*moduleHandle, "DbgPrint", 0, 0);
	if(!procAddress) {
		return false;
	}

	auto newNtQueryInformationProcess = m_processManagement32.injectData(shellcode2);
	if(!newNtQueryInformationProcess) {
		return false;
	}

	return createHook((std::uint32_t)*procAddress, *newNtQueryInformationProcess,1);
}

