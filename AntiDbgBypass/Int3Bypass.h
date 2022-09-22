#pragma once
#include <optional>
#include "BaseBypass32.h"

class Int3Bypass : public BaseBypass32 {
	enum BreakPointType {
		SingleInt3 = 1,
		Int3WithCatch
	};

	using IndexRange = std::pair<std::size_t, std::size_t>;

public:
	using BaseBypass32::BaseBypass32;
	bool bypass();

private:
	std::vector<std::uint32_t> getBreakPointsLocations();
	std::vector<std::uint32_t> getBreakPointsOverMemoryRange(std::uint32_t beginVa, std::uint32_t size);
	std::vector<std::uint32_t> removeUnnecessaryBreakPoints(std::vector<std::uint32_t>& breakPoints);
	std::vector<IndexRange> getCCTrains(std::vector<std::uint32_t>& breakPoints);
	std::vector<IndexRange> removeAlignTrains(std::vector<std::uint32_t>& breakPoints, std::vector<IndexRange> trains);
	std::optional<BreakPointType> analyzeBreakPoint(std::uint32_t va);
	bool overrideBreakPoint(std::uint32_t va, BreakPointType breakPointType);
};

