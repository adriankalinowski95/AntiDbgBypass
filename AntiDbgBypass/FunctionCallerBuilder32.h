#pragma once

#include <cstdint>
#include "FunctionCallerBuilder.h"

class FunctionCallerBuilder32 : public FunctionCallerBuilder<std::uint32_t> {
public:
	using FunctionCallerBuilder::FunctionCallerBuilder<std::uint32_t>;

};