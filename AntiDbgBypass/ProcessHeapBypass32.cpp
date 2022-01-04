#include "ProcessHeapBypass32.h"

ProcessHeapBypass32::ProcessHeapBypass32(ProcessManagement32& processManegement32): BaseBypass32(processManegement32) {}

bool ProcessHeapBypass32::bypass() {
	return false;
}
