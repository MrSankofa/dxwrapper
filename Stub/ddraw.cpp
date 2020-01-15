/**
* Copyright (C) 2020 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include <windows.h>
#include "..\Wrappers\wrapper.h"

#undef VISIT_ALL_PROCS
#define VISIT_ALL_PROCS(visit) \
	VISIT_PROCS_DDRAW(visit) \
	VISIT_PROCS_DDRAW_SHARED(visit)

#define DLL_NAME "\\ddraw.dll"

#define CREATE_WRAPPED_PROC(procName, unused) \
	FARPROC m_p ## procName = nullptr;

#define INITIALIZE_WRAPPED_PROC(procName, unused) \
	m_p ## procName = (FARPROC)GetProcAddress(dll, #procName); \
	ddraw::procName ## _var = m_p ## procName;

#define REDIRECT_WRAPPED_PROC(procName, unused) \
	ddraw::procName ## _var = (FARPROC)*dd_ ## procName;

#define CREATE_PROC_STUB(procName, unused) \
	extern "C" __declspec(naked) void __stdcall dd_ ## procName() \
	{ \
		__asm mov edi, edi \
		__asm cmp IsLoaded, 0 \
		__asm jne NEAR AsmExit \
		__asm call InitAsm \
		__asm AsmExit: \
		__asm jmp m_p ## procName \
	}

namespace DdrawWrapper
{
	bool IsLoaded = false;

	char dllname[MAX_PATH];

	VISIT_ALL_PROCS(CREATE_WRAPPED_PROC);

	void InitDll()
	{
		// Load dll
		HMODULE dll = LoadLibraryA(dllname);

		// Get function addresses
		VISIT_ALL_PROCS(INITIALIZE_WRAPPED_PROC);

		// Mark ddraw as loaded
		IsLoaded = true;
	}

	__declspec(naked) void __stdcall InitAsm()
	{
		__asm
		{
			mov edi, edi
			push eax
			push ebx
			push ecx
			push edx
			push esi
			push edi
			call InitDll
			pop edi
			pop esi
			pop edx
			pop ecx
			pop ebx
			pop eax
			ret
		}
	}

	VISIT_ALL_PROCS(CREATE_PROC_STUB);

	void Start(const char *name)
	{
		if (name)
		{
			strcpy_s(dllname, MAX_PATH, name);
		}
		else
		{
			GetSystemDirectoryA(dllname, MAX_PATH);
			strcat_s(dllname, DLL_NAME);
		}

		VISIT_ALL_PROCS(REDIRECT_WRAPPED_PROC);
	}
}
