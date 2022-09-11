#include "ProcessStructures32UM.h"
#include "ProcessManagementUtils.h"
#include <memory>

std::optional<std::uint64_t> ProcessStructures32UM::getPEBVa() {
	auto pbi = getPBI();
	if(!pbi) {
		return std::nullopt;
	}

	return reinterpret_cast<std::uint32_t>( pbi->PebBaseAddress ) + ProcessManagementUtils::Page_Size;
}

PEBAbstraction* ProcessStructures32UM::getPEBByVa(std::uint64_t va) {
	//PEB32C peb32{,va }
	PEB32 peb32{};
	if(!m_vmm.getVar(peb32, va)) {
		return nullptr;
	}

	return new PEB32C{peb32, va};
	//return std::make_shared<PEB32C>(peb32, va).get();
}