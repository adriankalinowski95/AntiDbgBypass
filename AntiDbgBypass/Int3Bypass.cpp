#include "Int3Bypass.h"
#include "BreakPointOverride.h"
#include <array>
#include <numeric>

bool Int3Bypass::bypass() {
	auto breakPoints = getBreakPointsLocations();
	if(breakPoints.empty()) {
		return true;
	}

	auto trains = getCCTrains(breakPoints);
	// We need to remove unnecessary trains

	trains = removeAlignTrains(breakPoints, trains);

	auto currentBreakPoints = convertTrainToBreakPoints(breakPoints, trains);
	BreakPointOverride bpOverride(m_processManagement);
	
	for(auto& breakPoint : currentBreakPoints) {
		bpOverride.overrideByRaisException(breakPoint);
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
	for(auto train : trains) {
		auto lastBreakPoint = breakPoints[train.first];
		std::uint8_t lastByte{};
		if (!this->m_processManagement.getVmm().getVar(lastByte, lastBreakPoint - 0x1)) {
				continue;
		}

		if (lastByte == 0xC3) {
			continue;
		}

		auto trainSize = static_cast<int>(train.second - train.first) + 1;
		if (trainSize >= sizeof(std::uint32_t)) {
			continue;
		}

		ranges.push_back(train);
	}

	return ranges;
}

std::vector<std::uint32_t> Int3Bypass::convertTrainToBreakPoints(std::vector<std::uint32_t>& breakPoints, std::vector<IndexRange>& trains)
{
	return std::vector<std::uint32_t>();
}
