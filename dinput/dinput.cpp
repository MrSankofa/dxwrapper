/**
* Copyright (C) 2019 Elisha Riedlinger
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

#include "dinput8\dinput8External.h"
#include "External\dinputto8\resource.h"
#include "External\dinputto8\dinputto8.h"

#define INITIALIZE_WRAPPED_PROC(procName, unused) \
	FARPROC procName ## _out = nullptr;

namespace DinputWrapper
{
	FARPROC DirectInput8Create_out = nullptr;
	FARPROC DllCanUnloadNow_out = nullptr;
	FARPROC DllGetClassObject_out = nullptr;
	FARPROC DllRegisterServer_out = nullptr;
	FARPROC DllUnregisterServer_out = nullptr;
}

using namespace DinputWrapper;

DWORD diVersion = 0;

AddressLookupTableDinput<void> ProxyAddressLookupTable = AddressLookupTableDinput<void>();

HRESULT WINAPI di_DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID * lplpDD, LPUNKNOWN punkOuter);

HRESULT WINAPI di_DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN punkOuter)
{
	return di_DirectInputCreateEx(hinst, dwVersion, IID_IDirectInputA, (LPVOID*)lplpDirectInput, punkOuter);
}

HRESULT WINAPI di_DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* lplpDirectInput, LPUNKNOWN punkOuter)
{
	return di_DirectInputCreateEx(hinst, dwVersion, IID_IDirectInputW, (LPVOID*)lplpDirectInput, punkOuter);
}

HRESULT WINAPI di_DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID * lplpDD, LPUNKNOWN punkOuter)
{
	static DirectInput8CreateProc m_pDirectInput8Create = (Wrapper::ValidProcAddress(DirectInput8Create_out)) ? (DirectInput8CreateProc)DirectInput8Create_out : nullptr;

	if (!m_pDirectInput8Create || !lplpDD)
	{
		return E_FAIL;
	}

	static bool RunOnce = true;
	if (RunOnce)
	{
		Logging::Log() << "Starting dinputto8 v" << APP_VERSION;
		RunOnce = false;
	}

	Logging::Log() << "Redirecting 'DirectInputCreate' " << riid << " version " << Logging::hex(dwVersion) << " to --> 'DirectInput8Create'";

	HRESULT hr = m_pDirectInput8Create(hinst, 0x0800, (GetStringType(riid) == DEFAULT_CHARSET) ? IID_IDirectInput8W : IID_IDirectInput8A, lplpDD, punkOuter);

	if (SUCCEEDED(hr))
	{
		diVersion = dwVersion;
		genericQueryInterface(riid, lplpDD);
	}

	return hr;
}

HRESULT WINAPI di_DllCanUnloadNow()
{
	static DllCanUnloadNowProc m_pDllCanUnloadNow = (Wrapper::ValidProcAddress(DllCanUnloadNow_out)) ? (DllCanUnloadNowProc)DllCanUnloadNow_out : nullptr;

	if (!m_pDllCanUnloadNow)
	{
		return E_FAIL;
	}

	return m_pDllCanUnloadNow();
}

HRESULT WINAPI di_DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	static DllGetClassObjectProc m_pDllGetClassObject = (Wrapper::ValidProcAddress(DllGetClassObject_out)) ? (DllGetClassObjectProc)DllGetClassObject_out : nullptr;

	if (!m_pDllGetClassObject || !ppv)
	{
		return E_FAIL;
	}

	DWORD StringType = GetStringType(riid);

	HRESULT hr = m_pDllGetClassObject(rclsid, (StringType == ANSI_CHARSET) ? IID_IDirectInput8A : (StringType == DEFAULT_CHARSET) ? IID_IDirectInput8W : riid, ppv);

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riid, ppv);
	}

	return hr;
}

HRESULT WINAPI di_DllRegisterServer()
{
	static DllRegisterServerProc m_pDllRegisterServer = (Wrapper::ValidProcAddress(DllRegisterServer_out)) ? (DllRegisterServerProc)DllRegisterServer_out : nullptr;

	if (!m_pDllRegisterServer)
	{
		return E_FAIL;
	}

	return m_pDllRegisterServer();
}

HRESULT WINAPI di_DllUnregisterServer()
{
	static DllUnregisterServerProc m_pDllUnregisterServer = (Wrapper::ValidProcAddress(DllUnregisterServer_out)) ? (DllUnregisterServerProc)DllUnregisterServer_out : nullptr;

	if (!m_pDllUnregisterServer)
	{
		return E_FAIL;
	}

	return m_pDllUnregisterServer();
}
