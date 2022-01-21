#pragma once
#include "ProcessManagement32.h"
#include <vector>

class Hook32 {
using Shellcode = std::pair<std::uint32_t, std::vector<std::uint8_t>>;

struct Overwrite {
	std::vector<std::uint8_t> oldData;
	std::vector<std::uint8_t> newData;
};

public:
	Hook32(ProcessManagement32& processManagement);
	bool createHook(std::uint32_t overwriteVa, std::uint32_t hookFunctionVa, std::uint8_t paramsCount);
	bool overwriteNtQueryInformationProcess();

private:
	ProcessManagement32& m_processManagement32;

	bool injectJumpShellcode(std::uint32_t overwriteVa, std::uint32_t detourVa);
	std::optional<Shellcode> getDetour(std::uint32_t hookFunctionVa, std::uint8_t paramsCount, std::uint32_t returnAddress, std::vector<uint8_t>& overwritedCode);
	std::vector<std::uint8_t> getOverwritedCode(std::uint32_t overwriteVa, uint32_t size);
	std::uint32_t getJumpShellcodeSize();
	std::vector<std::uint8_t> getParamsPointers(std::uint8_t paramsCount);
	std::vector<std::uint8_t> getBranchChanges(std::uint8_t prefix, std::uint32_t functionVa, std::uint32_t overwritedVa);
	std::vector<std::uint8_t> getJumpOverwrite(std::uint32_t functionVa, std::uint32_t overwritedVa);
	std::vector<std::uint8_t> getCallOverwrite(std::uint32_t functionVa, std::uint32_t overwritedVa);

	constexpr static std::uint32_t Jump_Shellcode_Size = 5;
	constexpr static std::uint32_t Call_Shellcode_Size = 5;
	constexpr static std::uint32_t Jump_Back_Shellcode_Size = 5;
	constexpr static std::uint8_t Nt_Query_Information_Process_Args = 5;
};

