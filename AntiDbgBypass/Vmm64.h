#pragma once
#include "Vmm.h"

class Vmm64 : public Vmm<std::uint64_t> {
	public:
		using Vmm::Vmm;
};