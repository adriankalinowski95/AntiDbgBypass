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

	virtual std::optional<T> allocMemory(T size, std::uint32_t protect) {
		return std::nullopt;
	}

	virtual bool freeMemory(T address) {
		return false;
	}

	virtual bool protectMemory(T address, T dwSize, T flNewProtect, T* lpflOldProtect) {
		return false;
	}

	std::optional<HANDLE> getProcessHandle() {
		return m_processHandle;
	}

	static constexpr std::uint32_t Page_Size = 0x1000;
protected:
	std::optional<HANDLE> m_processHandle;
};
