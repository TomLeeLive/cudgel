#pragma once
#include "GDxHelper.h"
#include "GCamera.h"
using namespace DX;

class GMiniMap : public GCamera
{
public:
	shared_ptr<GShape>		m_pPlaneShape;
	shared_ptr<GDxRT>		m_pMiniMapRT;
	D3D11_VIEWPORT			m_vp;
public:
	void		Set(ID3D11Device* pDevice, FLOAT TopLeftX, FLOAT TopLeftY, FLOAT Width, FLOAT Height,
		FLOAT MinDepth = 0, FLOAT MaxDepth = 1);
	bool		Create(ID3D11Device* pd3dDevice,TCHAR* pLoadShaderFile = 0,	TCHAR* pLoadTexture = 0);
	bool		Frame();
	bool		Render(ID3D11DeviceContext*    pContext);
	bool		Release();
	bool		BeginRender(ID3D11DeviceContext*    pContext, 
							D3DXVECTOR4 vColor = D3DXVECTOR4(1,1,1,1));
	bool		EndRender(ID3D11DeviceContext*    pContext);
	HRESULT		CreateResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext*    pContext, UINT iWidth, UINT iHeight);
	HRESULT		DeleteResource();
public:
	GMiniMap(void);
	virtual ~GMiniMap(void);
};
