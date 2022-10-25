#pragma once

#include <memory>
#include "IFunctionCaller.h"


template <class Address_Size>
class FunctionCallerBuilder {
	class IFunctionCaller<Address_Size>;
public:	
	FunctionCallerBuilder(Address_Size callerVa, Address_Size functionVa) :
		functionCaller{ callerVa, functionVa } {}
	
	template <class FunctionCaller>
	std::shared_ptr<IFunctionCaller<Address_Size>> build() {
		return std::make_shared<FunctionCaller>(m_functionCaller);
	}

	FunctionCallerBuilder& withArgs(std::vector<Address_Size>& args) {
		m_functionCaller.m_args = args;

		return *this;
	}

	FunctionCallerBuilder& withTrampoline(bool withTrampoline) {
		m_functionCaller.m_withTrampoline = withTrampoline;

		return *this;
	}

private:
	IFunctionCaller<Address_Size> m_functionCaller;
};
