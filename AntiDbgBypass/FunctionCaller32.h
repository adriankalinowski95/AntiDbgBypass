#pragma once

#include "IFunctionCaller.h"
#include "FunctionCallerBuilder.h"

class FunctionCaller32 : public IFunctionCaller<std::uint32_t> {
	friend FunctionCallerBuilder<std::uint32_t>;
public:
	std::vector<std::uint8_t> generateFunctionCaller() override;
};

