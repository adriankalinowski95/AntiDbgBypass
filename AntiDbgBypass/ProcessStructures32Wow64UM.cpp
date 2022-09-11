#include "ProcessStructures32Wow64UM.h"
#include "ProcessManagementUtils.h"
#include "PEB64C.h"

std::optional<std::uint64_t> ProcessStructures32Wow64UM::getPEBVa() {
	auto pbi = getPBI();
	if(!pbi) {
		return std::nullopt;
	}

	return reinterpret_cast<std::uint64_t>( pbi->PebBaseAddress );
}

PEBAbstraction* ProcessStructures32Wow64UM::getPEBByVa(std::uint64_t va) {
	//PEB32C peb32{,va }
	PEB64 peb64{};
	if(!m_vmm.getVar(peb64, va)) {
		return nullptr;
	}
	
	return new PEB64C{ peb64, va };
	//return std::make_shared<PEB32C>(peb32, va).get();
}
