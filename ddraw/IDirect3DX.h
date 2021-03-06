#pragma once

#include "IDirectDrawX.h"

#define D3DDEVICEDESC1_SIZE 172
#define D3DDEVICEDESC5_SIZE 204
#define D3DDEVICEDESC6_SIZE 252

class m_IDirect3DX : public IUnknown, public AddressLookupTableDdrawObject
{
private:
	IDirect3D7 *ProxyInterface = nullptr;
	DWORD ProxyDirectXVersion;
	ULONG RefCount = 1;
	m_IDirectDrawX *ddrawParent = nullptr;

	// Store d3d version wrappers
	m_IDirect3D *WrapperInterface;
	m_IDirect3D2 *WrapperInterface2;
	m_IDirect3D3 *WrapperInterface3;
	m_IDirect3D7 *WrapperInterface7;

	// Wrapper interface functions
	REFIID GetWrapperType(DWORD DirectXVersion)
	{
		return (DirectXVersion == 1) ? IID_IDirect3D :
			(DirectXVersion == 2) ? IID_IDirect3D2 :
			(DirectXVersion == 3) ? IID_IDirect3D3 :
			(DirectXVersion == 7) ? IID_IDirect3D7 : IID_IUnknown;
	}
	bool CheckWrapperType(REFIID IID)
	{
		return (IID == IID_IDirect3D ||
			IID == IID_IDirect3D2 ||
			IID == IID_IDirect3D3 ||
			IID == IID_IDirect3D7) ? true : false;
	}
	IDirect3D *GetProxyInterfaceV1() { return (IDirect3D *)ProxyInterface; }
	IDirect3D2 *GetProxyInterfaceV2() { return (IDirect3D2 *)ProxyInterface; }
	IDirect3D3 *GetProxyInterfaceV3() { return (IDirect3D3 *)ProxyInterface; }
	IDirect3D7 *GetProxyInterfaceV7() { return ProxyInterface; }

	// Interface initialization functions
	void InitDirect3D();
	void ReleaseDirect3D();

	// Resolution hack
	void ResolutionHack();

public:
	m_IDirect3DX(IDirect3D7 *aOriginal, DWORD DirectXVersion) : ProxyInterface(aOriginal)
	{
		ProxyDirectXVersion = GetGUIDVersion(ConvertREFIID(GetWrapperType(DirectXVersion)));

		if (ProxyDirectXVersion != DirectXVersion)
		{
			LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << "(" << this << ")" << " converting interface from v" << DirectXVersion << " to v" << ProxyDirectXVersion);
		}
		else
		{
			LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << "(" << this << ") v" << DirectXVersion);
		}

		InitDirect3D();

		ProxyAddressLookupTable.SaveAddress(this, (ProxyInterface) ? ProxyInterface : (void*)this);
	}
	m_IDirect3DX(m_IDirectDrawX *lpDdraw, DWORD DirectXVersion) : ddrawParent(lpDdraw)
	{
		ProxyDirectXVersion = 9;

		LOG_LIMIT(3, "Creating interface " << __FUNCTION__ << "(" << this << ")" << " converting interface from v" << DirectXVersion << " to v" << ProxyDirectXVersion);

		InitDirect3D();

		ProxyAddressLookupTable.SaveAddress(this, (ProxyInterface) ? ProxyInterface : (void*)this);
	}
	~m_IDirect3DX()
	{
		LOG_LIMIT(3, __FUNCTION__ << "(" << this << ")" << " deleting interface!");

		ReleaseDirect3D();

		ProxyAddressLookupTable.DeleteAddress(this);
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj, DWORD DirectXVersion);
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) { return QueryInterface(riid, ppvObj, GetGUIDVersion(riid)); }
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirect3D methods ***/
	STDMETHOD(Initialize)(THIS_ REFCLSID);
	HRESULT EnumDevices(LPD3DENUMDEVICESCALLBACK, LPVOID);
	HRESULT EnumDevices7(LPD3DENUMDEVICESCALLBACK7, LPVOID, bool = false);
	STDMETHOD(CreateLight)(THIS_ LPDIRECT3DLIGHT*, LPUNKNOWN);
	STDMETHOD(CreateMaterial)(THIS_ LPDIRECT3DMATERIAL3*, LPUNKNOWN, DWORD);
	STDMETHOD(CreateViewport)(THIS_ LPDIRECT3DVIEWPORT3*, LPUNKNOWN, DWORD);
	STDMETHOD(FindDevice)(THIS_ LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);
	STDMETHOD(CreateDevice)(THIS_ REFCLSID, LPDIRECTDRAWSURFACE7, LPDIRECT3DDEVICE7*, LPUNKNOWN, DWORD);
	STDMETHOD(CreateVertexBuffer)(THIS_ LPD3DVERTEXBUFFERDESC, LPDIRECT3DVERTEXBUFFER7*, DWORD, LPUNKNOWN, DWORD);
	STDMETHOD(EnumZBufferFormats)(THIS_ REFCLSID, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);
	STDMETHOD(EvictManagedTextures)(THIS);

	// Helper functions
	void *GetWrapperInterfaceX(DWORD DirectXVersion);

	// Functions handling the ddraw parent interface
	void SetDdrawParent(m_IDirectDrawX *ddraw) { ddrawParent = ddraw; }
	void ClearDdraw() { ddrawParent = nullptr; }
};
