#pragma once
#include <string>
#include <optional>
#include <Windows.h>

class LoaderBase {
public:
	LoaderBase(std::string processName) : 
		m_processName{processName},
		m_handle{std::nullopt} {}
	
	virtual ~LoaderBase(){}

	std::string getProcessName() {
		return m_processName;
	}

	std::optional<HANDLE> getProcessHandle() {
		return m_handle;
	}

	void setProcessHandle(HANDLE handle) {
		m_handle.emplace(handle);
	}

private:
	std::string m_processName;
	std::optional<HANDLE> m_handle;
};