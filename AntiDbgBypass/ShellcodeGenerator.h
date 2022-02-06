#pragma once
#include <vector>

class ShellcodeGenerator {
public:
	ShellcodeGenerator(std::uint32_t baseAddress);

	static std::vector<std::uint8_t> getParamsPointers(std::uint8_t paramsCount);
	static std::vector<std::uint8_t> getBranchChanges(std::vector<std::uint8_t> prefix, std::uint32_t shellcodeSize, std::uint32_t destinationVa, std::uint32_t overwritedVa);
	static std::vector<std::uint8_t> getJmp(std::uint32_t destinationVa, std::uint32_t overwritedVa);
	static std::vector<std::uint8_t> getCall(std::uint32_t destinationVa, std::uint32_t overwritedVa);
	static std::vector<std::uint8_t> getJe(std::uint32_t destinationVa, std::uint32_t overwritedVa);
	static std::vector<std::uint8_t> getJne(std::uint32_t destinationVa, std::uint32_t overwritedVa);
	static std::vector<std::uint8_t> getCmp(std::uint32_t value);
	static std::vector<std::uint8_t> getMovEax(std::uint32_t value);
	static std::vector<std::uint8_t> getRetN(std::uint16_t value);
	static std::vector<std::uint8_t> convUint32ToVec(std::uint32_t value);
	
	std::uint32_t getCurrentOffset();
	std::vector<std::uint8_t> getShellcode();
	void putRawBytes(std::vector<std::uint8_t>& bytes);
	void putPushEbx();
	void putPopEbx();
	void putParamsPointetrs(std::uint8_t paramsCount);
	void putJmp(std::uint32_t destinationVa);
	void putCall(std::uint32_t destinationVa);
	void putJe(std::uint32_t destinationVa);
	void putJne(std::uint32_t destinationVa);
	void putCmp(std::uint32_t value);
	void putMovEax(std::uint32_t value);
	void putRetN(std::uint16_t value);

	constexpr static std::uint8_t Push_Ebx = 0x53;
	constexpr static std::uint8_t Pop_Ebx = 0x5B;
	constexpr static std::uint32_t Jmp_Size = 5;
	constexpr static std::uint32_t Call_Size = 5;
	constexpr static std::uint32_t Je_Size = 6;
	constexpr static std::uint32_t Jne_Size = 6;
	constexpr static std::uint32_t Cmp_Size = 5;
	constexpr static std::uint32_t Mov_Eax_Size = 5;
	constexpr static std::uint32_t Ret_N_Size = 3;

private:
	std::uint32_t m_baseAddress;
	std::vector<std::uint8_t> m_shellcode;
};

