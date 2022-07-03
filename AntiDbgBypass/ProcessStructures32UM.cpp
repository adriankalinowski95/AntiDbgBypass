#include "ProcessStructures32UM.h"
#include "ProcessManagementUtils.h"

std::optional<PEB32> ProcessStructures32UM::getPEB32() {
	auto peb32Va = getPEB32Va();
	if(!peb32Va) {
		return std::nullopt;
	}

	return getPEB32ByVa(*peb32Va);
}

std::optional<std::uint32_t> ProcessStructures32UM::getPEB32Va() {
	auto pbi = getPBI();
	if(!pbi) {
		return std::nullopt;
	}

	return reinterpret_cast<std::uint32_t>( pbi->PebBaseAddress ) + ProcessManagementUtils::Page_Size;
}
