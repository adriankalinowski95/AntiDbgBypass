#pragma once
#include "ProcessManagement.h"
#include "ProcessManagementUtils.h"
#include <optional>

class ProcessManagement32 {
public:
	ProcessManagement32(ProcessManagement<std::uint32_t>& processManagment);
	~ProcessManagement32();
	std::optional<IMAGE_NT_HEADERS32> getNtHeader();
	std::optional<PEB> getPEB();

private:
	ProcessManagement<std::uint32_t>& m_processManagement;

	static constexpr std::uint32_t Page_Size = 0x1000;
};	