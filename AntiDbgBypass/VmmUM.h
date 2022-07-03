#pragma once
#include "VmmBase.h"

template <typename T>
class VmmUM : public VmmBase<T> {
public:
	using VmmBase<T>::VmmBase;
	using VmmBase<T>::m_loader;
	using VmmBase<T>::get_module_information;

	std::optional<std::int32_t> getModuleInformation(get_module_information& mod) {
		return std::nullopt;
	}

	std::optional<std::int32_t> readMemory(T address, std::uint8_t* buffer, T size) {
		if(!m_loader.getProcessHandle()) {
			return std::nullopt;
		}

		SIZE_T readedBytes{};
		if(!ReadProcessMemory(*m_loader.getProcessHandle(), reinterpret_cast<LPCVOID>( address ), reinterpret_cast<LPVOID>( buffer ), size, &readedBytes)) {
			return std::nullopt;
		}

		return static_cast<std::int32_t>( readedBytes );
	}

	std::optional<std::int32_t> writeMemory(T address, std::uint8_t* buffer, std::uint32_t size) {
		if(!m_loader.getProcessHandle()) {
			return std::nullopt;
		}

		SIZE_T writedBytes{};
		if(!WriteProcessMemory(*m_loader.getProcessHandle(), reinterpret_cast<LPVOID>( address ), reinterpret_cast<LPVOID>( buffer ), size, &writedBytes)) {
			return std::nullopt;
		}

		return static_cast<std::int32_t>( writedBytes );
	}

	std::optional<T> allocMemory(std::uint32_t size, std::uint32_t protect) {
		if(!m_loader.getProcessHandle()) {
			return std::nullopt;
		}

		return (T)VirtualAllocEx(*m_loader.getProcessHandle(), NULL, size, MEM_COMMIT | MEM_RESERVE, protect);
	}

	bool freeMemory(T address) {
		if(!m_loader.getProcessHandle()) {
			return false;
		}

		return VirtualFreeEx(*m_loader.getProcessHandle(), (LPVOID)address, 0, MEM_RELEASE);
	}

	bool protectMemory(T address, uint32_t dwSize, uint32_t flNewProtect, uint32_t* lpflOldProtect) {
		if(!m_loader.getProcessHandle()) {
			return false;
		}

		return VirtualProtectEx(*m_loader.getProcessHandle(), (LPVOID)address, dwSize, flNewProtect, (PDWORD)lpflOldProtect);
	}
};