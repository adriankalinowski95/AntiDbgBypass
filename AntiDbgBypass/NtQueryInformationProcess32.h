#pragma once
#include "BaseBypass32.h"

class NtQueryInformationProcess32 : BaseBypass32 {
public:
	NtQueryInformationProcess32(ProcessManagement32& processManegement32);
	bool bypass();
	bool overwriteNtQueryInformationProcess();

private:
	static constexpr std::uint8_t NtQueryInformationProcess_Args = 5;
};
