#pragma once

#include <vector>
#include <cstdint>

template <class Address_Size>
class IFunctionCaller {
public:
	virtual std::vector<std::uint8_t> generateFunctionCaller() = 0;

	auto getCallerVa()  {
		return m_callerVa;
	}

	auto getFunctionVa() {
		return m_functionVa;
	}

	auto getArgsCount() {
		retrun m_argsCount;
	}

	auto isWithTrapoline() {
		return m_withTrampoline;
	}

protected:
	Address_Size m_callerVa;
	Address_Size m_functionVa;
	size_t m_argsCount;
	bool m_withTrampoline;
};