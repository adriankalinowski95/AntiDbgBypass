#pragma once

#include <vector>
#include <cstdint>

#include "FunctionCallerBuilder.h"

template <class Address_Size>
class IFunctionCaller {					   
	friend FunctionCallerBuilder<Address_Size>;
public:
	virtual std::vector<std::uint8_t> generateFunctionCaller() = 0;

	IFunctionCaller(Address_Size callerVa, Address_Size functionVa) : 
		m_callerVa{ callerVa },
		m_functionVa{ functionVa },
		m_args{},
		m_withTrampoline{}{}

	IFunctionCaller() : 
		m_callerVa{},
		m_functionVa{},
		m_args{},
		m_withTrampoline{} {}

	auto getCallerVa()  {
		return m_callerVa;
	}

	auto getFunctionVa() {
		return m_functionVa;
	}

	auto getArgsCount() {
		return m_args;
	}

	auto isWithTrapoline() {
		return m_withTrampoline;
	}

protected:
	Address_Size m_callerVa;
	Address_Size m_functionVa;
	std::vector<Address_Size> m_args;
	bool m_withTrampoline;
};