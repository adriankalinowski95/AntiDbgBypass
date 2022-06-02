#pragma once
#include "BaseBypass32.h"

class Heap32 : BaseBypass32 {
public:
	Heap32(ProcessManagement32& processManagement);
	bool bypass();

private:
	bool isGlobalFlagsClearInProcess();
	std::vector<std::uint32_t> getSignaturesFromHeap(HEAP32& heap, std::uint32_t signature);
	std::vector<std::uint32_t> getAntiDbgSignatures();
	std::vector<std::uint32_t> getAntiDbgSignaturesWow64();
	std::vector<std::uint32_t> getSignaturesFromBlock(const ProcessManagement32::HeapBlock32& heapBlock, std::uint32_t signature);
	bool overwriteSignatures(std::vector<std::uint32_t>& signatures);

	static constexpr std::uint32_t AntiDbg_Signature_1 = 0xABABABAB;
	static constexpr std::uint32_t AntiDbg_Signature_2 = 0xFEEEFEEE;

	bool bypassX86();
	bool bypassWowX64();
};

	