#pragma once
#include <string>
#include <cstdint>
#include <optional>

template<typename T>
class ProcessManagement {
public:
	struct get_module_information {
		T imageBase;
		T sizeOfImage;
	};

	virtual std::optional<std::int32_t> getModuleInformation(get_module_information& mod) = 0;
	virtual std::optional<std::int32_t> readMemory(T base, T* buffer, std::uint32_t size) = 0;
	virtual std::optional<std::int32_t> writeMemory(T base, T* buffer, std::uint32_t size) = 0;
};