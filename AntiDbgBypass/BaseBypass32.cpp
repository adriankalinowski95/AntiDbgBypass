#include "BaseBypass32.h"

BaseBypass32::BaseBypass32(ProcessManagement32& processManagement):m_processManagement{ processManagement }{}

bool BaseBypass32::bypass() {
	return false;
}
