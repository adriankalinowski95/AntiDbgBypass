#pragma once
#include <string>
#include <cstdint>
#include <optional>
#include <Windows.h>
#include "peb.h"

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

	std::optional<PROCESS_BASIC_INFORMATION> getPBI() {
		BOOL isWow64 = FALSE;
		if(!IsWow64Process(m_processManagement.getProcessHandle().value(), &isWow64)) {
			std::nullopt;
		}

		if(!isWow64) {
			return std::nullopt;
		}

		PROCESS_BASIC_INFORMATION pbi;
		if(NtQueryInformationProcess(m_processManagement.getProcessHandle().value(), ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL) != 0) {
			return std::nullopt;
		}

		return pbi;
	}

	/*
	std::optional<> getPEB() {
		if(!m_processManagement.getProcessHandle()) {
			return std::nullopt;
		}

		auto pbi = getPBI();
		if(!pbi) {
			return std::nullopt;
		}

		PEB32 peb{};
		const auto readedMemory = m_processManagement.readMemory(getPEB32FromPBI(*pbi), reinterpret_cast<std::uint8_t*>( &peb ), sizeof(peb));
		if(!readedMemory || readedMemory != sizeof(peb)) {
			return std::nullopt;
		}

		return peb;
	}
	*/

	static constexpr std::uint32_t Page_Size = 0x1000;
protected:
	std::optional<HANDLE> m_processHandle;
};
