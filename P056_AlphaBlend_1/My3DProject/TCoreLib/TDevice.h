#pragma once
#include "TStd.h"
#include "TDxHelperEx.h"
using namespace DX;

class TDevice
{
public:
	TDxRT       m_DefaultRT;
	HWND		m_hWnd; // 윈도우 핸들
	HINSTANCE	m_hInstance; // 실행객체 핸들
	RECT		m_rtWindow;
	DWORD		m_dwWidth;
	DWORD		m_dwHeight;
	D3D11_VIEWPORT m_vp;
public:
	D3D_DRIVER_TYPE         g_driverType;
	D3D_FEATURE_LEVEL       g_featureLevel;
	ID3D11Device*           m_pd3dDevice;
	ID3D11DeviceContext*    g_pImmediateContext;
	ID3D11DeviceContext*  GetContext() 
	{
		return g_pImmediateContext;	
	}
	IDXGISwapChain*         g_pSwapChain;
		
	IDXGIFactory*			g_pGIFactory;
	DXGI_SWAP_CHAIN_DESC	m_sd;
public:
	bool   ResizeDevice(UINT iWidth, UINT iHeight);
	HRESULT SetRenderTarget();
	HRESULT SetViewPort();
	HRESULT CreateGIFactory();
	HRESULT InitSwapChain();
	HRESULT InitDevice();
	void	CleanupDevice();
	HRESULT  TDevice::CreateDXResouce();
	HRESULT  TDevice::DeleteDXResouce();
public:
	TDevice();
	virtual ~TDevice();
};

