#pragma once
#include <string>
#include <optional>
#include <Windows.h>

class LoaderBase {
public:
	LoaderBase(std::string processName) : 
		m_processName{processName},
		m_handle{std::nullopt},
		m_processId {std::nullopt} {}
	
	virtual ~LoaderBase(){}

	std::string getProcessName() {
		return m_processName;
	}

	std::optional<HANDLE> getProcessHandle() {
		return m_handle;
	}

	std::optional<std::uint32_t> getProcessId() {
		return m_processId;
	}

	void setProcessHandle(HANDLE handle) {
		m_handle.emplace(handle);
	}

	void setProcessId(std::uint32_t newProcessId) {
		m_processId.emplace(newProcessId);
	}

private:
	std::string m_processName;
	std::optional<HANDLE> m_handle;
	std::optional<std::uint32_t> m_processId;
};