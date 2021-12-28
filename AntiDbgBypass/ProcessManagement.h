#pragma once
#include <string>
#include <cstdint>
#include <optional>
#include <Windows.h>

template<typename T>
class ProcessManagement {
public:
	struct get_module_information {
		T imageBase;
		T sizeOfImage;
	};

	virtual std::optional<std::int32_t> getModuleInformation(get_module_information& mod) {
		return std::nullopt;
	}
	virtual std::optional<std::int32_t> readMemory(T base, std::uint8_t* buffer, std::uint32_t size) {
		return std::nullopt;
	}

	virtual std::optional<std::int32_t> writeMemory(T base, std::uint8_t* buffer, std::uint32_t size) {
		return std::nullopt;
	}

	std::optional<HANDLE> getProcessHandle() {
		return m_processHandle;
	}

protected:
	std::optional<HANDLE> m_processHandle;
};
