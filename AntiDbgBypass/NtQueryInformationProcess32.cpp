#include "NtQueryInformationProcess32.h"
#include "RemoteOps.h"
#include "Hook32.h"

// Debug flag
// Debug object
// Debug port

/*
std::vector<uint8_t> NtQueryInformationProcessShellcode = 
{ 0x55,0x8b,0xec,0x8b,0x45,0x0c,0x83,0x38,0x07,0x75,
	0x09,0x8b,0x4d,0x0c,0xc7,0x01,0x00,0x00,0x00,0x00,
0x5d,0xc2,0x14,0x00 };
*/
std::vector<uint8_t> NtQueryInformationProcessShellcode2 =
{0x55,0x8b,0xec,0x8b,0x45,0x0c,0x83,0x38,0x07,0x74,
0x10,0x8b,0x4d,0x0c,0x83,0x39,0x1f,0x74,0x08,0x8b,
0x55,0x0c,0x83,0x3a,0x1e,0x75,0x09,0x8b,0x45,0x0c,
0xc7,0x00,0x00,0x00,0x00,0x00,0xb8,0x01,0x00,0x00,
0x00,0x5d,0xc2,0x14,0x00};


NtQueryInformationProcess32::NtQueryInformationProcess32(ProcessManagement32& processManegement32): BaseBypass32(processManegement32) {}

bool NtQueryInformationProcess32::bypass() {
	return overwriteNtQueryInformationProcess();
}

bool NtQueryInformationProcess32::overwriteNtQueryInformationProcess() {
	auto moduleHandle = m_processManagement.getRemoteModuleHandle("ntdll.dll");
	if(!moduleHandle) {
		return false;
	}

	auto procVa = m_processManagement.getRemoteProcAddress(*moduleHandle, "NtQueryInformationProcess", 0, 0);
	if(!procVa) {
		return false;
	}

	auto injectedDataVa = m_processManagement.injectData(NtQueryInformationProcessShellcode2);
	if(!injectedDataVa) {
		return false;
	}

	Hook32 hook32{ m_processManagement };
	return hook32.createHook((std::uint32_t)*procVa, *injectedDataVa, NtQueryInformationProcess_Args);
}