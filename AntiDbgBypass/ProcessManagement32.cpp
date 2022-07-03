#include "ProcessManagement32.h"
#include <Windows.h>
#include "psapi.h"
#include "RemoteOps.h"
#include <optional>

ProcessManagement32::ProcessManagement32(VmmBase<std::uint32_t>& vmm, ProcessStructures32Base& processStructures):
					ProcessManagement{ vmm },
					m_processStructures{ processStructures } {}

std::optional<std::uint32_t> ProcessManagement32::injectData(std::vector<std::uint8_t>& data) {
	std::uint32_t allocSize = data.size();
	if(allocSize % ProcessManagementUtils::Page_Size) {
		allocSize += ProcessManagementUtils::Page_Size - ( allocSize % ProcessManagementUtils::Page_Size );
	}

	auto injectAreaVa = m_vmm.allocMemory(allocSize, PAGE_EXECUTE_READWRITE);
	if(!injectAreaVa) {
		return std::nullopt;
	}

	if(!m_vmm.writeMemory(*injectAreaVa, data.data(), data.size())) {
		return false;
	}

	return injectAreaVa;
}

ProcessStructures32Base& ProcessManagement32::getStructures() {
	return m_processStructures;
}
