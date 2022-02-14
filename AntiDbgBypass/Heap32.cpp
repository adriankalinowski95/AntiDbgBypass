#include "Heap32.h"

Heap32::Heap32(ProcessManagement32& processManagement):BaseBypass32(processManagement) {}

bool Heap32::bypass() {
	auto basicBlcoks = m_processManagement.getBasicBlocks();

	return false;
}
