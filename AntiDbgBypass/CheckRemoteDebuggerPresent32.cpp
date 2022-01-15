#include "CheckRemoteDebuggerPresent32.h"
#include "RemoteOps.h"
#include "Hook32.h"
CheckRemoteDebuggerPresent32::CheckRemoteDebuggerPresent32(ProcessManagement32& processManegement32): BaseBypass32(processManegement32) {}

bool CheckRemoteDebuggerPresent32::bypass() {
	// RemoteOps::GetRemoteModuleHandle(m_pro,"ntdll.dll")
	// auto moduleHandle = m_processManagement.getRemoteModuleHandle("ntdll.dll");
	// auto procAddress = m_processManagement.getRemoteProcAddress(*moduleHandle,"NtQueryInformationProcess",0,0);
	Hook32 hook32{m_processManagement};
	hook32.overwriteNtQueryInformationProcess();
	// hook32.overwriteNtQueryInformationProcess();

	return false;
}
