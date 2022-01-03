#pragma once
#include "BaseBypass32.h"

class NtGlobalFlag32 : public BaseBypass32 {
public:
	NtGlobalFlag32(ProcessManagement32& processManagement);
	bool bypass();

private:
	bool isNtGlobalFlag32();
	bool isNtGlobalFlag64();
	bool bypassNtGlobalFlag32();
	bool bypassNtGlobalFlag64();

	static constexpr std::uint32_t FLG_HEAP_ENABLE_TAIL_CHECK = 0x10;
	static constexpr std::uint32_t FLG_HEAP_ENABLE_FREE_CHECK = 0x20;
	static constexpr std::uint32_t FLG_HEAP_VALIDATE_PARAMETERS = 0x40;
	static constexpr std::uint32_t NT_GLOBAL_FLAG_DEBUGGED = ( FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS );
};

