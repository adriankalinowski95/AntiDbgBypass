#include "CheckRemoteDebuggerPresent32.h"
#include "RemoteOps.h"
#include "Hook32.h"
CheckRemoteDebuggerPresent32::CheckRemoteDebuggerPresent32(ProcessManagement32& processManegement32): BaseBypass32(processManegement32) {}

bool CheckRemoteDebuggerPresent32::bypass() {
	Hook32 hook32{m_processManagement};
	hook32.overwriteNtQueryInformationProcess();

	return false;
}
