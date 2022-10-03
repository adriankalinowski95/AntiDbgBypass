#pragma once

#include "PEBBase.h"
#include "peb.h"

class PEB64C : public PEBBase<PEB64> {
public:
	using PEBBase<PEB64>::PEBBase;
};