#include "Int3Bypass.h"
#include <array>
#include <numeric>

bool Int3Bypass::bypass() {
	auto breakPoints = getBreakPointsLocations();
	if(breakPoints.empty()) {
		return true;
	}

	auto trains = getCCTrains(breakPoints);
	// We need to remove unnecessary trains

	for(auto& breakPoint : breakPoints) {
		const auto breakPointType = analyzeBreakPoint(breakPoint);
		if(!breakPointType) {
			continue;
		}

		overrideBreakPoint(breakPoint, *breakPointType);
	}

	return false;
}

std::vector<std::uint32_t> Int3Bypass::getBreakPointsLocations() {
	const auto moduleEntry = m_processManagement.getStructures().getModuleEntry();
	if(!moduleEntry) {
		return std::vector<std::uint32_t>();
	}
	
	auto moduleBaseVa = moduleEntry->getModBaseVa();
	auto moduleSize = moduleEntry->getModBaseSize();
	if (!moduleBaseVa || !moduleSize) {
		return std::vector<std::uint32_t>();
	}

	return getBreakPointsOverMemoryRange(moduleBaseVa, moduleSize);
}

std::vector<std::uint32_t> Int3Bypass::getBreakPointsOverMemoryRange(std::uint32_t beginVa, std::uint32_t size) {
	if(!beginVa || ( beginVa + size ) < beginVa || !size) {
		return std::vector<std::uint32_t>();
	}
	
	std::vector<std::uint32_t> breakPoints{};
	std::array<std::uint8_t, 0x1000> tempArray{};

	std::uint32_t iterates = size / 0x1000 + ((size % 0x1000) ? 1 : 0);
	auto tempSize = size;
	for (auto i = 0u; i < iterates; i++) {
		std::uint32_t currSize = (tempSize > 0x1000) ? 0x1000 : tempSize;

		auto readedSize = this->m_processManagement.getVmm().readMemory(beginVa + i * 0x1000, tempArray.data(), currSize);
		if(!readedSize) {
			continue;
		}

		if(readedSize > 0x1000) {
			continue;
		}

		std::accumulate(tempArray.begin(), tempArray.end(), 0, [&breakPoints, &beginVa, &i](int& index, std::uint8_t& val) -> int {
			if(val == 0xCC) {
				breakPoints.push_back(beginVa + 0x1000 * i + index);
			}

			return index + 1;
		});

		tempSize -= *readedSize;
	}

	return breakPoints;
}

std::vector<std::uint32_t> Int3Bypass::removeUnnecessaryBreakPoints(std::vector<std::uint32_t>& breakPoints) {
	return std::vector<std::uint32_t>();
}

std::vector<Int3Bypass::IndexRange> Int3Bypass::getCCTrains(std::vector<std::uint32_t>& breakPoints) {
	if(breakPoints.empty()) {
		return std::vector<IndexRange>();
	}

	std::vector<IndexRange> ranges{};
	std::uint32_t lastVa = breakPoints[0];
	size_t beginIndex{};
	for(auto i = 0u; i < breakPoints.size(); i++) {	
		if(( i + 1 ) >= breakPoints.size()) {
			IndexRange range{ beginIndex, i };
			ranges.push_back(range);

			break;
		}

		if(( lastVa + 1 ) == breakPoints[i + 1]) {
			lastVa += 1;
			
			continue;
		}

		IndexRange range{ beginIndex, i };
		ranges.push_back(range);
		beginIndex = i + 1;
		lastVa = breakPoints[i + 1];
	}

	return ranges;
}

std::vector<Int3Bypass::IndexRange> Int3Bypass::removeAlignTrains(std::vector<std::uint32_t>& breakPoints, std::vector<IndexRange> trains) {
	if(breakPoints.empty() || trains.empty()) {
		return std::vector<IndexRange>();
	}

	std::vector<IndexRange> ranges{};



	return std::vector<IndexRange>();
}

std::optional<Int3Bypass::BreakPointType> Int3Bypass::analyzeBreakPoint(std::uint32_t va) {
	return std::optional<BreakPointType>();
}

bool Int3Bypass::overrideBreakPoint(std::uint32_t va, BreakPointType breakPointType) {
	return false;
}
