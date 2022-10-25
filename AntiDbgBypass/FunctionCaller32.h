#pragma once

#include "IFunctionCaller.h"
// #include "FunctionCallerBuilder.h"

class FunctionCaller32 : public IFunctionCaller<std::uint32_t> {
	// friend FunctionCallerBuilder<std::uint32_t>;
public:
	FunctionCaller32(std::uint32_t a, std::uint32_t b) : 
		IFunctionCaller<std::uint32_t>(a,b) {}

	std::vector<std::uint8_t> generateFunctionCaller() override;
};
