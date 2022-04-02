#include "Heap32.h"
#include <iostream>

Heap32::Heap32(ProcessManagement32& processManagement):BaseBypass32(processManagement) {}

bool Heap32::bypass() {
	auto heaps = m_processManagement.getHeaps();
	if(heaps.empty()) {
		return false;
	}
	
	for(auto i = 0; i < heaps.size(); i++) {
		const auto heapBlocks = m_processManagement.getHeapBlocks(heaps[i]);
		if(heapBlocks.empty()) {
			std::cout << "Heap without blocks ! " << std::endl;

			continue;
		}

		auto overlappedVa = heapBlocks[i].first + heapBlocks[i].second.Size + sizeof(std::uint32_t);
		std::uint32_t overlappedVal{};
		if (!m_processManagement.getVmm().getVar(overlappedVal, overlappedVa)) {
			return false;
		}
		
		if(overlappedVal == 0xABABABAB) {
			std::cout << "Heap overlapped 0xABABABAB detected! " << std::endl;
		}
	}

	return false;
}
