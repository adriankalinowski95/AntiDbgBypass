#pragma once

#include "IModuleEntry.h"

template <class T>
class ModuleEntryBase : public IModuleEntry {
public:
	ModuleEntryBase(T moduleEntry, std::uint64_t va):
		m_moduleEntry{ moduleEntry },
		m_va{ va } {}

	std::uint64_t getDwSize() override {
		return m_moduleEntry.dwSize;
	}

	std::uint64_t getGlblcntUsage() override {
		return m_moduleEntry.GlblcntUsage;
	}

	std::uint64_t getProccentUsage() override {
		return m_moduleEntry.ProccntUsage;
	}

	std::uint64_t getModBaseVa() override {
		return reinterpret_cast<std::uint64_t>(m_moduleEntry.modBaseAddr);
	}

	std::uint64_t getModBaseSize() override {
		return m_moduleEntry.modBaseSize;
	}

	HMODULE getHModule() override {
		return m_moduleEntry.hModule;
	}

	std::optional<std::pair<std::uint64_t, std::uint64_t>> getCodeRange() {
		auto rangeStart = (std::uint64_t)(m_moduleEntry.modBaseAddr);
		auto rangeEnd = (std::uint64_t)(m_moduleEntry.modBaseAddr) + (std::uint64_t)m_moduleEntry.modBaseSize;
		if (rangeStart == rangeEnd || rangeStart > rangeEnd) {
			return std::nullopt;
		}

		return std::make_pair(rangeStart, rangeEnd);
	}

protected:
	T m_moduleEntry;
	std::uint64_t m_va;
};