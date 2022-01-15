#include "hooklib.hpp"
#include <algorithm>
#include <cassert>

#pragma region ignore me
#include <fstream>
#include <ctime>
#include <sstream>
#include <algorithm>

char * GetTime( )
{
	auto now = std::time( 0 );
	std::tm out = { 0 };
	auto local_now = localtime_s( &out, &now );

	char * out_str = new char[ 100 ]( );
	sprintf_s( out_str, 100, "%02d:%02d:%02d", out.tm_hour, out.tm_min, out.tm_sec );
	return out_str;
}

void LogToFile( const char * format, ... )
{
	//static bool bFirst = false;
	static std::ofstream * pFile = nullptr;
	if ( pFile == nullptr )
	{
		char file_name[ 256 ] = { 0 };
		if ( GetModuleFileNameA( GetModuleHandleA( 0 ), file_name, sizeof( file_name ) ) == 0 )
		{
			Log( "COULDNT GET FILE NAME, U WOT" );
		}
		else
		{
			std::string fname( file_name );
			std::stringstream ss;
			auto name_spot = fname.find_last_of( "\\" );
			auto real_name = fname.substr( name_spot + 1 );
			name_spot = real_name.find_last_of( "." );
			real_name = real_name.substr( 0, name_spot );

			ss << "DEBUG_LOG_" << real_name << ".log";

			Log( "Creating file %s", ss.str( ).c_str( ) );

			pFile = new std::ofstream( ss.str( ).c_str( ), std::ios::trunc | std::ofstream::out );
		}
	}

	va_list vl;
	va_start( vl, format );
	char out_string[ 250 ] = { 0 };

	vsnprintf_s( out_string, sizeof( out_string ), format, vl );

	va_end( vl );

	( *pFile ) << out_string;
	pFile->flush( );

}
#pragma endregion

// ye im lazy fuck u want
#define PS(loc, size) \
	DWORD proc = 0;\
	VirtualProtect((void*)loc, size, PAGE_EXECUTE_READWRITE, &proc)
#define PE(loc, size )\
	VirtualProtect((void*)loc, size, proc, &proc )	


namespace Utility
{
	bool bCompare( const char * pData, const char * bMask, const char * szMask )
	{
		for ( ; *szMask; ++szMask, ++pData, ++bMask )
			if ( *szMask == 'x' && *pData != *bMask )
				return false;
		return ( *szMask ) == NULL;
	}

	uintptr_t FindPattern( uintptr_t dwAddress, uintptr_t dwLen, const char * bMask, const char * szMask )
	{
		if ( dwAddress == 0 )
		{
			//Log( "Start address is null" );
			return 0;
		}

		for ( uintptr_t i = 0; i < dwLen; i++ )
			if ( bCompare( ( char * )( dwAddress + i ), bMask, szMask ) )
				return ( uintptr_t )( dwAddress + i );

		return 0;
	}

	uintptr_t FindPattern( uintptr_t dwAddress, uintptr_t dwLen, const char * pat )
	{
		std::string pattern( pat ), bytePattern, byteMask;
		// AA BB CC DD EE ?? FF GG ? ZZ
		// do this past each space. strtol will stop at that point.
		auto loc = 0;
		while ( loc != pattern.npos )
		{
			// case for ?? / ?
			if ( *( pattern.c_str( ) + loc ) == '?' )
				bytePattern += '\x00', byteMask += '?';
			else
				bytePattern += strtol( pattern.c_str( ) + loc, 0, 16 ), byteMask += 'x';

			auto f = pattern.find( ' ', loc );
			loc = f != pattern.npos ? f + 1 : f; //npos or space + 1
		}

		return FindPattern( dwAddress, dwLen, bytePattern.c_str( ), byteMask.c_str( ) );
	}
}

void Utility::HookStruct::SetOriginalBytes( char * oBytes )
{
	this->original_bytes = oBytes;
}

void Utility::HookStruct::SetHookSize( char size )
{
	this->hook_size = size;
}

char * Utility::HookStruct::GetOriginalBytes( )
{
	return this->original_bytes;
}

char Utility::HookStruct::GetHookSize( )
{
	return this->hook_size;
}

void Utility::HookManager::PatchByte( uintptr_t Target, char byte, int length )
{
	PS( Target, length );

	memset( ( void * )Target, byte, length );

	PE( Target, length );
}

void Utility::HookManager::PatchBytes( uintptr_t Target, char * bytes, int length )
{
	PS( Target, length );

	memcpy( ( void * )Target, bytes, length );

	PE( Target, length );
}

uintptr_t Utility::HookManager::HookFunction( uintptr_t Target, uintptr_t Hook, int size, bool call )
{
	puts( "You probably want to use HookFunctionExt." );

	// 5 - 0 = 5 - 5 = 0
	auto displacement = ( Hook - Target ) - 5; // 5 = length of a JMP
	if ( size && size < 5 )
	{
		// tough shit, not coded yet. If you wanna add functionality: do a short JMP somewhere with at least 5 bytes of space.
	}
	else if ( size == 0 )
		size = 5;

	HookStruct * hs = new HookStruct( Target, Hook );
	hs->SetHookSize( size );
	char * oBytes = new char[ size ];
	memcpy( oBytes, ( void * )Target, size );
	hs->SetOriginalBytes( oBytes );
	this->HookedFunctions.push_back( hs );
	char * trampoline = 0;
	hs->Trampoline = trampoline;
	trampoline = ( char * )VirtualAlloc( 0, size + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE ); // pushes, pops, overwritten bytes, call, and jmp back
	if ( !trampoline )
	{
		puts( "Idk how you fucekd this up." );
		return 0;
	}

	memcpy( trampoline, oBytes, size );
	*( trampoline + size ) = 0xE9;
	auto loc = ( uintptr_t )trampoline + size;
	*( uintptr_t * )( loc + 1 ) = ( ( Target + size ) - loc ) - 5;

	PS( Target, size );
	if ( call )
		*( char * )Target = 0xE8;
	else
		*( char * )Target = 0xE9;

	*( int * )( Target + 1 ) = displacement;
	if ( size > 5 )
		memset( ( void * )( Target + 5 ), 0x90, size - 5 );
	PE( Target, size );

	return ( uintptr_t )trampoline;
}
#define TFX 1

bool Utility::HookManager::HookFunctionExt( uintptr_t Target, uintptr_t Hook, int size, bool execute_first, bool call )
{

	// Using a trampoline to
#ifdef TFX
	const char push_regs[ ] = "\x54\x9C\x50\x51\x52\x53\x56\x57\x55\x54";

	/*
		cmp eax, 0xdeadc0de
		jne skip
		nop
		skip:
		push eax
		popfd
		nop
		pushfd
		pop edx
		sub esp, edx
		ret
	*/
	const char skip_check[ ] = "\x3D\xDE\xC0\xAD\xDE\x75"; // append a byte == sizeof(pop_regs)-1 + jmp
	const char write_eax_mem[ ] = "\xA2\x00\x00\x00\x00"; // mov [$+0], al
	const char ret_num[ ] = "\xC2\x00\x00"; // ret 0

	const char pop_regs[ ] = "\x5C\x5D\x5F\x5E\x5B\x5A\x59\x58\x9D\x5C";
	if ( size < 5 )
		size = 5;
	// tough shit, not coded yet. If you wanna add functionality: do a short JMP somewhere with at least 5 bytes of space.

	HookStruct * hs = new HookStruct( Target, Hook );
	hs->SetHookSize( size );
	char * oBytes = new char[ size ];
	memcpy( oBytes, ( void * )Target, size );
	hs->SetOriginalBytes( oBytes );

	char * trampoline = 0;
	hs->Trampoline = trampoline;

	trampoline = ( char * )VirtualAlloc( 0, sizeof( push_regs ) + sizeof( pop_regs ) + size + 20, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE ); // pushes, pops, overwritten bytes, call, and jmp back
	if ( !trampoline )
	{
		puts( "Idk how you fucekd this up." );
		return 0;
	}

	if ( execute_first )
	{
		memcpy( trampoline, ( void * )Target, size );
		memcpy( trampoline + size, push_regs, sizeof( push_regs ) - 1 );
		auto hook_call = trampoline + size + sizeof( push_regs ) - 1; // execute overwritten bytes, push registers on stack

		auto displacement_2 = ( Hook - ( uintptr_t )hook_call ) - 5;
		*( char * )( hook_call ) = 0xE8;
		*( int * )( hook_call + 1 ) = displacement_2; // CALL our hook functionw here we can then edit registers and  whatnot

		auto post_hook = hook_call + 5;
		memcpy( post_hook, skip_check, sizeof( skip_check ) - 1 );
		*( post_hook + sizeof( skip_check ) - 1 ) = ( sizeof( pop_regs ) - 1 ) + 5; // pop regs -> jump back to OG function.
		memcpy( post_hook + sizeof( skip_check ), pop_regs, sizeof( pop_regs ) - 1 ); // add pop regs to this block.

		auto jump_back = post_hook + sizeof( skip_check ) + sizeof( pop_regs ) - 1;
		auto displacement = ( ( Target + size ) - ( uintptr_t )jump_back ) - 5;

		*( char * )( jump_back ) = 0xE9;
		*( int * )( jump_back + 1 ) = displacement; // jump back to hooked function

		auto skip_now = jump_back + 5;
		memcpy( skip_now, write_eax_mem, sizeof( write_eax_mem ) - 1 );
		memcpy( skip_now + sizeof( write_eax_mem ) - 1, pop_regs, sizeof( pop_regs ) - 1 );

		auto restore_stack = skip_now + sizeof( write_eax_mem ) + sizeof( pop_regs ) - 2;
		memcpy( restore_stack, ret_num, sizeof( ret_num ) - 1 );

		*( uint32_t * )( skip_now + 1 ) = ( ( uint32_t )restore_stack + 1 );
	}
	else
	{
		memcpy( trampoline, push_regs, sizeof( push_regs ) - 1 );
		auto hook_call = trampoline + sizeof( push_regs ) - 1; // execute overwritten bytes, push registers on stack

		auto displacement_2 = ( Hook - ( uintptr_t )hook_call ) - 5;
		*( char * )( hook_call ) = 0xE8;
		*( int * )( hook_call + 1 ) = displacement_2; // CALL our hook functionw here we can then edit registers and  whatnot

		auto post_hook = hook_call + 5;
		memcpy( post_hook, skip_check, sizeof( skip_check ) - 1 );
		*( post_hook + sizeof( skip_check ) - 1 ) = ( sizeof( pop_regs ) - 1 ) + size + 5; // pop regs -> jump back to OG function.
		memcpy( post_hook + sizeof( skip_check ), pop_regs, sizeof( pop_regs ) - 1 ); // add pop regs to this block.

		memcpy( post_hook + sizeof( skip_check ) + sizeof( pop_regs ) - 1, ( void * )Target, size ); // OG BYTES
		auto jump_back = post_hook + sizeof( skip_check ) + sizeof( pop_regs ) - 1 + size;
		auto displacement = ( ( Target + size ) - ( uintptr_t )jump_back ) - 5;
		*( char * )( jump_back ) = 0xE9;
		*( int * )( jump_back + 1 ) = displacement; // jump back to hooked function

		auto skip_now = jump_back + 5;
		memcpy( skip_now, write_eax_mem, sizeof( write_eax_mem ) - 1 );
		memcpy( skip_now + sizeof( write_eax_mem ) - 1, pop_regs, sizeof( pop_regs ) - 1 );

		auto restore_stack = skip_now + sizeof( write_eax_mem ) + sizeof( pop_regs ) - 2;
		memcpy( restore_stack, ret_num, sizeof( ret_num ) - 1 );

		*( uint32_t * )( skip_now + 1 ) = ( ( uint32_t )restore_stack + 1 );
		//////////////////////
	}

	PS( Target, size );
	auto displacement3 = ( ( uintptr_t )trampoline - Target ) - 5;
	*( char * )Target = 0xE9;
	*( int * )( Target + 1 ) = displacement3; // Hook target function to trampoline
	if ( size > 5 )
		memset( ( void * )( Target + 5 ), 0x90, size - 5 );

	PE( Target, size );

	//Log( "Trampoline at %p", trampoline );

	this->HookedFunctions.push_back( hs );

#else
#define MRAX_XMM0 "\x66\x48\x0f\x7E\xc0\x50" // includes push rax
#define MRAX_XMM1 "\x66\x48\x0f\x7E\xc8\x50"
#define MRAX_XMM2 "\x66\x48\x0f\x7E\xd0\x50"
#define MRAX_XMM3 "\x66\x48\x0f\x7E\xd8\x50"

#define MXMM0_RAX "\x58\x66\x48\x0f\x6E\xc0"
#define MXMM1_RAX "\x58\x66\x48\x0f\x6E\xc8"
#define MXMM2_RAX "\x58\x66\x48\x0f\x6E\xd0"
#define MXMM3_RAX "\x58\x66\x48\x0f\x6E\xd8"

	const char push_regs[ ] = "\x54\x9C\x41\x57\x41\x56\x41\x55\x41\x54\x41\x53\x41\x52\x41\x51\x41\x50\x50\x51\x52\x53\x56\x57\x55" MRAX_XMM0 MRAX_XMM1 MRAX_XMM2 MRAX_XMM3 "\x48\x89\xE1\x48\x83\xec\x48"; // also includes sub rsp 48 (extra 8 so we align stack to 0x10 boundary)
	const char pop_regs[ ] = "\x48\x83\xc4\x48" MXMM3_RAX MXMM2_RAX MXMM1_RAX MXMM0_RAX "\x5D\x5F\x5E\x5B\x5A\x59\x58\x41\x58\x41\x59\x41\x5A\x41\x5B\x41\x5C\x41\x5D\x41\x5E\x41\x5F\x9D\x5C"; // also includes add rsp 48
	const char call_rdx[ ] = "\x48\xBA\x66\x55\x44\x33\x22\x11\x00\x00\xFF\xD2"; // mov rdx, val, call rdx
	const char lazy_push[ ] = "\x68\x44\x33\x22\x11\xC7\x44\x24\x04\x88\x77\x66\x55\xC3"; // we push the lo-dword, set the hi-dword
	const char jump_hook[ ] = "\x50\x48\xB8\x77\x66\x55\x44\x33\x22\x11\x00\xFF\xE0"; // push rax, mov rax, jmp rax

	if ( size && size < 5 )
	{
		// tough shit, not coded yet. If you wanna add functionality: do a short JMP somewhere with at least 5 bytes of space.
	}
	else if ( size == 0 )
		size = 0xD;

	HookStruct * hs = new HookStruct( Target, Hook );
	hs->SetHookSize( size );
	char * oBytes = new char[ size ];
	memcpy( oBytes, ( void * )Target, size );
	hs->SetOriginalBytes( oBytes );

	char * trampoline = 0;
	hs->Trampoline = trampoline;

	trampoline = ( char * )VirtualAlloc( 0, sizeof( push_regs ) + sizeof( pop_regs ) + size + ( 13 * 2 ), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE ); // pushes, pops, overwritten bytes, call, and jmp back
	if ( !trampoline )
	{
		puts( "Idk how you fucekd this up." );
		return false;
	}
	*trampoline = 0x58; // add the pop rax in now.
	trampoline++; // dont fuck up calculations cause im lazy and already wrote em

	if ( execute_first )
	{
		memcpy( trampoline, ( void * )Target, size );
		memcpy( trampoline + size, push_regs, sizeof( push_regs ) - 1 );
		auto big_size = trampoline + size + sizeof( push_regs ) - 1; // execute overwritten bytes, push registers on stack

		memcpy( big_size, call_rdx, sizeof( call_rdx ) - 1 );
		*( uintptr_t * )( big_size + 2 ) = Hook;  // place hook addr.

		memcpy( big_size + sizeof( call_rdx ) - 1, pop_regs, sizeof( pop_regs ) - 1 ); // POP our changes into the registers
		big_size += ( sizeof( call_rdx ) - 1 ) + ( sizeof( pop_regs ) - 1 );

		// do a push-ret return.
		memcpy( big_size, lazy_push, sizeof( lazy_push ) - 1 );
		*( unsigned long * )( big_size + 1 ) = ( Target + size ) & 0xFFFFFFFF;
		*( unsigned long * )( big_size + 9 ) = ( ( Target + size ) >> 32 ) & 0xFFFFFFFF;
	}
	else
	{
		//memcpy( trampoline, ( void * )Target, size );
		memcpy( trampoline, push_regs, sizeof( push_regs ) - 1 );
		auto big_size = trampoline + sizeof( push_regs ) - 1; // push registers on stack

		memcpy( big_size, call_rdx, sizeof( call_rdx ) - 1 );
		*( uintptr_t * )( big_size + 2 ) = Hook;  // place hook addr.

		memcpy( big_size + sizeof( call_rdx ) - 1, pop_regs, sizeof( pop_regs ) - 1 ); // POP our changes into the registers
		big_size += ( sizeof( call_rdx ) - 1 ) + ( sizeof( pop_regs ) - 1 );

		// now we execute overwritten bytes.
		memcpy( big_size, ( void * )Target, size );
		big_size += size;

		// do a push-ret return.
		memcpy( big_size, lazy_push, sizeof( lazy_push ) - 1 );
		*( unsigned long * )( big_size + 1 ) = ( Target + size ) & 0xFFFFFFFF;
		*( unsigned long * )( big_size + 9 ) = ( ( Target + size ) >> 32 ) & 0xFFFFFFFF;
	}

	--trampoline; // back to the pop rax

	PS( Target, size );

	memcpy( ( void * )Target, jump_hook, sizeof( jump_hook ) - 1 );
	*( uintptr_t * )( Target + 3 ) = ( uintptr_t )trampoline;
	if ( size > 0xD )
		memset( ( void * )( Target + 0xD ), 0x90, size - 0xD );

	PE( Target, size );

	this->HookedFunctions.push_back( hs );
#endif

	return true;
}

uintptr_t Utility::HookManager::GetCallableOriginal( uintptr_t target, uintptr_t hook )
{
	auto fns = this->HookedFunctions.size( );
	for ( auto & x : this->HookedFunctions )
	{
		if ( x->TargetAddress == target || x->HookFunction == hook )
		{
			if ( x->CallableFunction )
				return x->CallableFunction;
			// generate callable.
			// bytes we overwrote + jump
			char * callable_loc = nullptr;

#ifndef _AMD64_
			callable_loc = ( char * )VirtualAlloc( 0, x->GetHookSize( ), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
			memcpy( callable_loc, x->GetOriginalBytes( ), x->GetHookSize( ) );
			*( callable_loc + x->GetHookSize( ) ) = 0xE9;
			auto cur_loc = ( uintptr_t )callable_loc + x->GetHookSize( );
			*( uint32_t * )( cur_loc + 1 ) = ( x->TargetAddress + x->GetHookSize( ) - cur_loc - 5 );
			return ( uintptr_t )callable_loc;
#else
			assert( false );
#endif
		}
	}

}

void Utility::HookManager::UnhookFunction( uintptr_t Target )
{
	auto foundFn = std::find_if( HookedFunctions.begin( ), HookedFunctions.end( ), [ Target ]( HookStruct * pStruct )
		{
			return ( pStruct->TargetAddress == Target );
		} );

	if ( foundFn != HookedFunctions.end( ) )
	{
		auto pStruct = *foundFn;
		PS( Target, pStruct->GetHookSize( ) );
		memcpy( ( void * )Target, pStruct->GetOriginalBytes( ), pStruct->GetHookSize( ) );
		PE( Target, pStruct->GetHookSize( ) );

		if ( pStruct->Trampoline )
		{
			VirtualFree( pStruct->Trampoline, 0, MEM_RELEASE );
		}

		HookedFunctions.erase( foundFn );
		delete pStruct;
	}
}

Utility::HookManager::~HookManager( )
{
	auto fns = this->HookedFunctions.size( );
	auto copy = this->HookedFunctions;
	for ( int i = 0; i < fns; i++ )
	{
		auto entry = copy.at( i );
		this->UnhookFunction( entry->TargetAddress );
		if ( entry->CallableFunction )
		{
#ifndef _AMD64_
			memset( ( void * )entry->CallableFunction, 0, entry->GetHookSize( ) + 5 );
			VirtualFree( ( void * )entry->CallableFunction, 0, MEM_RELEASE );
#else

#endif
		}
	}
}


#undef PS
#undef PE