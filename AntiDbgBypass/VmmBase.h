#pragma once
#include <optional>
#include <Windows.h>
#include "LoaderBase.h"

template <typename T>
class VmmBase {
public:
	VmmBase(LoaderBase& loaderBase): 
		m_loader{loaderBase} {}

	struct get_module_information {
		T imageBase;
		T sizeOfImage;
	};

	virtual std::optional<std::int32_t> getModuleInformation(get_module_information& mod) {
		return std::nullopt;
	}

	virtual std::optional<std::int32_t> readMemory(T base, std::uint8_t* buffer, std::uint32_t size) {
		return std::nullopt;
	}

	virtual std::optional<std::int32_t> writeMemory(T base, std::uint8_t* buffer, std::uint32_t size) {
		return std::nullopt;
	}

	virtual std::optional<T> allocMemory(T size, std::uint32_t protect) {
		return std::nullopt;
	}

	virtual bool freeMemory(T address) {
		return false;
	}

	virtual bool protectMemory(T address, T dwSize, T flNewProtect, T* lpflOldProtect) {
		return false;
	}
	
	auto& getLoader() {
		return m_loader;
	}

	template <class V>
	bool putVar(V& var, T address) {
		const auto status = writeMemory(address, reinterpret_cast<std::uint8_t*>( &var ), sizeof(V));
		if(!status) {
			return false;
		}

		return status.value() == sizeof(V);
	}

	template <class V>
	bool getVar(V& var, T address) {
		const auto status = readMemory(address, reinterpret_cast<std::uint8_t*>(&var), sizeof(V));
		if(!status) {
			return false;
		}

		return status.value() == sizeof(V);
	}

	bool getData(std::uint8_t* data, T size, T address) {
		const auto status = readMemory(address, data, size);
		if(!status) {
			return false;
		}

		return status.value() == size;
	}

	bool putData(std::uint8_t* data, T size, T address) {
		const auto status = writeMemory(address, data, size);
		if(!status) {
			return false;
		}

		return status.value() == size;
	}

protected:
	LoaderBase& m_loader;

	static constexpr std::uint32_t Page_Size = 0x1000;
};