#include "ThreadLocalStorage.h"

ThreadLocalStorage::ThreadLocalStorage(ProcessManagement32& processManagement):
	BaseBypass32(processManagement) {}

bool ThreadLocalStorage::bypass() {
	return false;
}
