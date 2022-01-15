#pragma once

#include "ProcessManagement.h"

class Vmm32 {
public:
	Vmm32(ProcessManagement<std::uint32_t>& processManagement);

	template <class T>
	bool putVar(T& var, std::uint32_t address) {
		const auto status = m_processManagement.writeMemory(address, ( std::uint8_t* )&var, sizeof(T));
		if(!status) {
			return false;
		}
		
		return status.value() == sizeof(T);
	}

	template <class T>
	bool getVar(T& var, std::uint32_t address) {
		const auto status = m_processManagement.readMemory(address, (std::uint8_t*)&var, sizeof(T));
		if(!status) {
			return false;
		}

		return status.value() == sizeof(T);
	}

	bool getData(std::uint8_t* data ,std::uint32_t size, std::uint32_t address) {
		const auto status = m_processManagement.readMemory(address, data, size);
		if(!status) {
			return false;
		}

		return status.value() == size;
	}


	bool putData(std::uint8_t* data, std::uint32_t size, std::uint32_t address) {
		const auto status = m_processManagement.writeMemory(address, data, size);
		if(!status) {
			return false;
		}

		return status.value() == size;
	}

	std::optional<std::uint32_t> allocMemory(std::uint32_t size, std::uint32_t protect) {
		return m_processManagement.allocMemory(size, protect);
	}

	bool protectMemory(uint32_t address, uint32_t dwSize, uint32_t flNewProtect, uint32_t* lpflOldProtect) {
		return m_processManagement.protectMemory(address, dwSize, flNewProtect, lpflOldProtect);
	}

private:
	ProcessManagement<std::uint32_t>& m_processManagement;
};