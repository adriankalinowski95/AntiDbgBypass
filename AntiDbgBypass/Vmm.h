#pragma once
#include <optional>
#include "ProcessManagement.h"

template<typename ArchBaseSize>
class Vmm 
{
public:
Vmm(ProcessManagement<ArchBaseSize>& processManagement): m_processManagement{processManagement} {}

template <class T>
bool putVar(T& var, ArchBaseSize address) {
	const auto status = m_processManagement.writeMemory(address, (std::uint8_t*)&var, sizeof(T));
	if(!status) {
		return false;
	}

	return status.value() == sizeof(T);
}

template <class T>
bool getVar(T& var, ArchBaseSize address) {
	const auto status = m_processManagement.readMemory(address, (std::uint8_t*)&var, sizeof(T));
	if(!status) {
		return false;
	}

	return status.value() == sizeof(T);
}

bool getData(std::uint8_t* data, ArchBaseSize size, ArchBaseSize address) {
	const auto status = m_processManagement.readMemory(address, data, size);
	if(!status) {
		return false;
	}

	return status.value() == size;
}


bool putData(std::uint8_t* data, ArchBaseSize size, ArchBaseSize address) {
	const auto status = m_processManagement.writeMemory(address, data, size);
	if(!status) {
		return false;
	}

	return status.value() == size;
}

std::optional<ArchBaseSize> allocMemory(ArchBaseSize size, ArchBaseSize protect) {
	return m_processManagement.allocMemory(size, protect);
}

bool protectMemory(ArchBaseSize address, uint32_t dwSize, uint32_t flNewProtect, uint32_t* lpflOldProtect) {
	return m_processManagement.protectMemory(address, dwSize, flNewProtect, lpflOldProtect);
}

protected:
	ProcessManagement<ArchBaseSize>& m_processManagement;
};