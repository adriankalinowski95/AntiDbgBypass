#include "ProcessStructures32UM.h"
#include "ProcessManagementUtils.h"
#include <memory>
#include "ModuleEntry32.h"

std::optional<std::uint64_t> ProcessStructures32UM::getPEBVa() {
	auto pbi = getPBI();
	if(!pbi) {
		return std::nullopt;
	}

	return reinterpret_cast<std::uint32_t>( pbi->PebBaseAddress ) + ProcessManagementUtils::Page_Size;
}

IPEB* ProcessStructures32UM::getPEBByVa(std::uint64_t va) {
	//PEB32C peb32{,va }
	PEB32 peb32{};
	if(!m_vmm.getVar(peb32, va)) {
		return nullptr;
	}

	return new PEB32C{peb32, va};
	//return std::make_shared<PEB32C>(peb32, va).get();
}

IModuleEntry* ProcessStructures32UM::getModuleEntry() {
	if(!m_loaderBase.getProcessHandle()) {
		return nullptr;
	}
	auto processName = m_loaderBase.getProcessName();
	DWORD processHandle = *m_loaderBase.getProcessId();
	std::wstring wProcessName(processName.begin(), processName.end());

	MODULEENTRY32 module32;
	module32.dwSize = sizeof(MODULEENTRY32); //Module32First will fail if you don't do this
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processHandle);
	Module32First(hSnap, &module32);
	if(hSnap == INVALID_HANDLE_VALUE) //just a check
		return 0;

	do //iterate through modules
	{
		std::wstring szModule(module32.szModule);
		if(szModule.compare(wProcessName) == 0) {
			CloseHandle(hSnap);

			return new ModuleEntry32(module32, 0);
		}
	} while(Module32Next(hSnap, &module32));
	CloseHandle(hSnap);

	return nullptr;
}
