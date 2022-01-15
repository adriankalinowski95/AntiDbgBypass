#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include <chrono>
#include <ctime>

char * GetTime( );

void LogToFile( const char * format, ... );

#define Log(format, ...) {\
	auto ptr = GetTime();\
	printf_s("[ %-20s | %d | %s ] "##format##"\n", __FUNCTION__, GetCurrentThreadId(), ptr , __VA_ARGS__);\
	delete ptr;}

#define LogF(format, ...) {\
auto ptr = GetTime();\
LogToFile("[ %-20s | %d | %s ] "##format##"\n", __FUNCTION__, GetCurrentThreadId(), ptr, __VA_ARGS__);\
delete ptr;}

#define LogB(format, ...) Log(format, __VA_ARGS__) LogF(format, __VA_ARGS__)

namespace Utility {

	enum TrampType
	{
		None,
		Basic,
		Extended // whether it hooks and copies bytes as a trampoline or not
	};

	struct HookStruct
	{
		uintptr_t TargetAddress, HookFunction = 0, CallableFunction = 0;
		char * Trampoline = 0;
		HookStruct( uintptr_t t, uintptr_t h ) : TargetAddress( t ), HookFunction( h ) { }

		void SetOriginalBytes( char * oBytes );
		void SetHookSize( char size );

		char * GetOriginalBytes( );
		char GetHookSize( );
	private:
		char * original_bytes;
		char hook_size;
	};

	// Use this structure as the only argument for your hook if you use HookFunctionExt. x86Registers *
	struct x86Registers
	{
		//int esp; // no esp modification for you
		int ebp;
		int edi;
		int esi;
		int ebx;
		int edx;
		int ecx;
		int eax;
		int eflags;
		void ** stack;
	};

	union XMMRegister
	{
		float fVal;
		double dVal;
	};

	struct x64Registers
	{
		//int esp; // no esp modification for you
		XMMRegister xmm3;
		XMMRegister xmm2;
		XMMRegister xmm1;
		XMMRegister xmm0; // first float/double.

		int64_t rbp;
		int64_t rdi;
		int64_t rsi;
		int64_t rbx;
		int64_t rdx;
		int64_t rcx;
		int64_t rax;
		int64_t r8;
		int64_t r9;
		int64_t r10;
		int64_t r11;
		int64_t r12;
		int64_t r13;
		int64_t r14;
		int64_t r15;

		int64_t rflags;
		void ** stack;
	};

#ifdef _AMD64_
	using registers = x64Registers;
#else
	using registers = x86Registers;
#endif

	class HookManager
	{
		std::vector<HookStruct *> HookedFunctions;

	public:
		const static int EXECUTE_TARGET_FUNCTION = 0xDEADC0DE;

		// Given an address, patch all bytes to given byte, for X length
		void PatchByte( uintptr_t Target, char byte, int length );
		// Given an address, patch given bytes, for X length (length necessary in case of a 0 in your bytes fuckers)
		void PatchBytes( uintptr_t Target, char * byte, int length );
		// Give an address, hook the function, nopping out for size-5 bytes after the jump. Replace JMP with CALL if requested
		uintptr_t HookFunction( uintptr_t Target, uintptr_t Hook, int size = 0, bool call = false );
		// Like above, however this gives you precise access to registers to modify in a much more precise way. (CALL NOT YET CODED)
		bool HookFunctionExt( uintptr_t Target, uintptr_t Hook, int size = 0, bool execute_first = true, bool call = false );
		// Get a callable pointer to execute the original function whilst youre in the hook (so you can inspect & modify return value without hooking elsewhere)
		uintptr_t GetCallableOriginal( uintptr_t target, uintptr_t hook = 0 );
		// Unhook a function based on its target address.
		void UnhookFunction( uintptr_t Target );
		~HookManager( );
	};

	// Initialize me ya dingus!
	extern HookManager * pHookManager;
	uintptr_t FindPattern( uintptr_t dwAddress, uintptr_t dwLen, const char * bMask, const char * szMask );
	uintptr_t FindPattern( uintptr_t dwAddress, uintptr_t dwLen, const char * pat );
}