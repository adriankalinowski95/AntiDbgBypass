#pragma once

#include "ProcessManagement.h"
#include "VmmUM.h"

class Vmm32UM : public VmmUM<std::uint32_t> {
public:
	using VmmUM<std::uint32_t>::VmmUM;
};