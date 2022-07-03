#include "Heap32.h"
#include <iostream>

// Rozkminic jak pisaæ kod, w taki spoósb aby dzia³a³ zarówno dla x86 i wow64
Heap32::Heap32(ProcessManagement32& processManagement):BaseBypass32(processManagement) {}

bool Heap32::bypass() {	
	auto allSignaturesVec = getAntiDbgSignatures();

	return overwriteSignatures(allSignaturesVec);
}

std::vector<std::uint32_t> Heap32::getSignaturesFromHeap(HEAP32& heap, std::uint32_t signature) {
	// Mo¿e  byætemplatowe
	const auto heapBlocks = m_processManagement.getStructures().getHeapBlocks(heap);
	if(heapBlocks.empty()) {
		return std::vector<std::uint32_t>();
	}

	std::vector<std::uint32_t> signatures{};
	for(auto& heapBlock : heapBlocks) {
		// mo¿e byætemplatowe
		auto siganturesFromBlock = getSignaturesFromBlock(heapBlock, signature);
		if(!siganturesFromBlock.empty()) {
			signatures.insert(end(signatures), begin(siganturesFromBlock), end(siganturesFromBlock));
		}
	}

	return signatures;
}

std::vector<std::uint32_t> Heap32::getAntiDbgSignatures() {
	std::vector<std::uint32_t> signatures{};

	auto heaps = m_processManagement.getStructures().getHeaps();
	if(heaps.empty()) {
		return signatures;
	}

	for(auto& heap : heaps) {
		// mozna zast¹pic
		const auto heapBlocks = m_processManagement.getStructures().getHeapBlocks(heap);
		if(heapBlocks.empty()) {
			std::cout << "Heap without blocks ! " << std::endl;

			continue;
		}
		std::vector<std::uint32_t> heapBlockSignatures{};

		for(auto& heapBlock: heapBlocks) {
			auto siganturesFromBlock = getSignaturesFromBlock(heapBlock, AntiDbg_Signature_1);
			if(!siganturesFromBlock.empty()) {
				heapBlockSignatures.insert(end(heapBlockSignatures), begin(siganturesFromBlock), end(siganturesFromBlock));
			}
		}

		for(auto& heapBlock : heapBlocks) {
			auto siganturesFromBlock = getSignaturesFromBlock(heapBlock, AntiDbg_Signature_2);
			if(!siganturesFromBlock.empty()) {
				heapBlockSignatures.insert(end(heapBlockSignatures), begin(siganturesFromBlock), end(siganturesFromBlock));
			}
		}

		std::cout << "Heap: " << std::hex << heap.BaseAddress << " signatures count: " << std::dec << heapBlockSignatures.size() << std::endl;
		std::cout << "Blocks count: " << heapBlocks.size() << std::endl;

		signatures.insert(end(signatures), begin(heapBlockSignatures), end(heapBlockSignatures));
	}

	return signatures;
}

std::vector<std::uint32_t> Heap32::getSignaturesFromBlock(const ProcessManagement32::HeapBlock32& heapBlock, std::uint32_t signature) {
	std::vector<std::uint32_t> signaturesVec{};
	auto dataCountToScan = heapBlock.second.Size / sizeof(std::uint32_t);
	for(auto i = 0u; i < dataCountToScan;i++) {
		auto overlappedVa = heapBlock.first +  sizeof(std::uint32_t) * i;
		std::uint32_t memVal{};
		if(!m_processManagement.getVmm().getVar(memVal, overlappedVa)) {
			std::cout << "Get signatures from block error " << std::endl;

			// return signaturesVec;
		}

		if(memVal == signature) {
			signaturesVec.push_back(overlappedVa);
		}
	}
	
	return signaturesVec;
}

bool Heap32::overwriteSignatures(std::vector<std::uint32_t>& signatures) {
	if(signatures.empty()) {
		return true;
	}

	std::uint32_t zeroVal{};
	for(auto& signatureVa : signatures) {
		if(!m_processManagement.getVmm().putVar(zeroVal, signatureVa)) {
			std::cout << "Overwrite false block error " << std::endl;

			return false;
		}
	}

	return true;
}