#include "CheckRemoteDebuggerPresent32.h"
#include "RemoteOps.h"
#include "Hook32.h"

CheckRemoteDebuggerPresent32::CheckRemoteDebuggerPresent32(ProcessManagement32& processManegement32): BaseBypass32(processManegement32) {}

bool CheckRemoteDebuggerPresent32::bypass() {
	return false;
}
