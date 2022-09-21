#pragma once
#include "ProcessManagement.h"
#include "ProcessManagementUtils.h"
#include "ProcessStructures32Base.h"
#include "VmmBase.h"
#include <optional>

class ProcessManagement32 : public ProcessManagement<std::uint32_t> {
public:
	using ImgLoadConfDir32_V = std::pair<std::uint32_t, IMAGE_LOAD_CONFIG_DIRECTORY32>;
	using HeapBlock32 = std::pair<std::uint32_t, HEAP_UNPACKED_PACKET_32>;

	ProcessManagement32(VmmBase<std::uint32_t>& vmm,LoaderBase& loaderBase, ProcessStructures32Base& processStructures);
	std::optional<std::uint32_t> injectData(std::vector<std::uint8_t>& data);
	ProcessStructures32Base& getStructures();
	
private:
	ProcessStructures32Base& m_processStructures;
};	