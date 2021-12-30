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


private:
	ProcessManagement<std::uint32_t>& m_processManagement;
};