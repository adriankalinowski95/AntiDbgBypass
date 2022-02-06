#include "ShellcodeGenerator.h"

ShellcodeGenerator::ShellcodeGenerator(std::uint32_t baseAddress):
	m_baseAddress{baseAddress},
	m_shellcode{}
	{}

std::vector<std::uint8_t> ShellcodeGenerator::getParamsPointers(std::uint8_t paramsCount) {
	std::vector<std::uint8_t> paramsPointers;
	std::vector<std::uint8_t> leaEcxEax = { 0x8D, 0x48, 0x04 };
	std::vector<std::uint8_t> movEaxEsp = { 0x89, 0xE0 };
	std::uint8_t pushEcx = 0x51;

	paramsPointers.insert(paramsPointers.end(), movEaxEsp.begin(), movEaxEsp.end());
	for(int i = 0; i < paramsCount;i++) {
		leaEcxEax[2] = paramsCount * 0x4 - 0x4 - i * 4;
		paramsPointers.insert(paramsPointers.end(), leaEcxEax.begin(), leaEcxEax.end());
		paramsPointers.push_back(pushEcx);
	}

	return paramsPointers;
}

std::vector<std::uint8_t> ShellcodeGenerator::getBranchChanges(std::vector<std::uint8_t> prefix,std::uint32_t shellcodeSize, std::uint32_t destinationVa, std::uint32_t overwritedVa) {
	std::vector<std::uint8_t> changeBranchInstr{};
	changeBranchInstr.insert(changeBranchInstr.end(), prefix.begin(), prefix.end());

	auto branchNearVa = destinationVa - overwritedVa - shellcodeSize;
	auto branchNearVec = convUint32ToVec(branchNearVa);
	changeBranchInstr.insert(changeBranchInstr.end(), branchNearVec.begin(), branchNearVec.end());

	return changeBranchInstr;
}

std::vector<std::uint8_t> ShellcodeGenerator::getJmp(std::uint32_t destinationVa, std::uint32_t overwritedVa) {
	return getBranchChanges({0xE9}, Jmp_Size, destinationVa, overwritedVa);
}

std::vector<std::uint8_t> ShellcodeGenerator::getCall(std::uint32_t destinationVa, std::uint32_t overwritedVa) {
	return getBranchChanges({ 0xE8 }, Call_Size, destinationVa, overwritedVa);
}

std::vector<std::uint8_t> ShellcodeGenerator::getJe(std::uint32_t destinationVa, std::uint32_t overwritedVa) {
	return getBranchChanges({ 0xF, 0x84 }, Je_Size, destinationVa, overwritedVa);
}

std::vector<std::uint8_t> ShellcodeGenerator::getJne(std::uint32_t destinationVa, std::uint32_t overwritedVa) {
	return getBranchChanges({ 0xF, 0x85 }, Jne_Size, destinationVa, overwritedVa);
}

std::vector<std::uint8_t> ShellcodeGenerator::getCmp(std::uint32_t value) {
	std::vector<std::uint8_t> cmp{};
	cmp.push_back(0x3D);
	auto cmpValueVec = convUint32ToVec(value);
	cmp.insert(cmp.end(), cmpValueVec.begin(), cmpValueVec.end());

	return cmp;
}

std::vector<std::uint8_t> ShellcodeGenerator::getMovEax(std::uint32_t value) {
	std::vector<std::uint8_t> movEax{};
	movEax.push_back(0xB8);
	auto movEaxValueVec = convUint32ToVec(value);
	movEax.insert(movEax.end(), movEaxValueVec.begin(), movEaxValueVec.end());

	return movEax;
}

std::vector<std::uint8_t> ShellcodeGenerator::getRetN(std::uint16_t value) {
	std::vector<std::uint8_t> retn(Ret_N_Size, 0);
	retn[0] = 0xC2;
	retn[1] = ( value ) & 0xFF;
	retn[2] = ( value >> 8 ) & 0xFF;

	return retn;
}

std::vector<std::uint8_t> ShellcodeGenerator::convUint32ToVec(std::uint32_t value) {
	std::vector<std::uint8_t> convVec{};
	convVec.push_back(( value ) & 0xFF);
	convVec.push_back(( value >> 8 ) & 0xFF);
	convVec.push_back(( value >> 16 ) & 0xFF);
	convVec.push_back(( value >> 24 ) & 0xFF);

	return convVec;
}

std::uint32_t ShellcodeGenerator::getCurrentOffset() {
	return m_baseAddress + m_shellcode.size();
}

std::vector<std::uint8_t> ShellcodeGenerator::getShellcode() {
	return m_shellcode;
}

void ShellcodeGenerator::putRawBytes(std::vector<std::uint8_t>& bytes) {
	m_shellcode.insert(m_shellcode.end(), bytes.begin(), bytes.end());
}

void ShellcodeGenerator::putPushEbx() {
	m_shellcode.push_back(Push_Ebx);
}

void ShellcodeGenerator::putPopEbx() {
	m_shellcode.push_back(Pop_Ebx);
}

void ShellcodeGenerator::putParamsPointetrs(std::uint8_t paramsCount) {
	auto instVec = getParamsPointers(paramsCount);
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}

void ShellcodeGenerator::putJmp(std::uint32_t destinationVa) {
	auto instVec = getJmp(destinationVa ,getCurrentOffset());
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}

void ShellcodeGenerator::putCall(std::uint32_t destinationVa) {
	auto instVec = getCall(destinationVa, getCurrentOffset());
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}

void ShellcodeGenerator::putJe(std::uint32_t destinationVa) {
	auto instVec = getJe(destinationVa, getCurrentOffset());
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}

void ShellcodeGenerator::putJne(std::uint32_t destinationVa) {
	auto instVec = getJne(destinationVa, getCurrentOffset());
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}

void ShellcodeGenerator::putCmp(std::uint32_t value) {
	auto instVec = getCmp(value);
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}

void ShellcodeGenerator::putMovEax(std::uint32_t value) {
	auto instVec = getMovEax(value);
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}

void ShellcodeGenerator::putRetN(std::uint16_t value) {
	auto instVec = getRetN(value);
	m_shellcode.insert(m_shellcode.end(), instVec.begin(), instVec.end());
}
