#include "GMiniMap.h"
void GMiniMap::Set(ID3D11Device* pDevice, FLOAT TopLeftX, FLOAT TopLeftY, FLOAT Width, FLOAT Height,
	FLOAT MinDepth, FLOAT MaxDepth)
{
	m_vp.TopLeftX = TopLeftX;
	m_vp.TopLeftY = TopLeftY;
	m_vp.Width = Width;
	m_vp.Height = Height;
	m_vp.MinDepth = MinDepth;
	m_vp.MaxDepth = MaxDepth;
}
bool GMiniMap::Create(ID3D11Device* pd3dDevice, TCHAR* pLoadShaderFile, TCHAR* pLoadTexture)
{
	m_pPlaneShape = make_shared<GPlaneShape>();
	if (FAILED(m_pPlaneShape->Create(pd3dDevice, pLoadShaderFile)))
	{
		MessageBox(0, _T("m_LineShape ½ÇÆÐ"), _T("Fatal error"), MB_OK);
		return 0;
	}

	m_pMiniMapRT = make_shared<GDxRT>();
	if (FAILED(m_pMiniMapRT->Create(pd3dDevice)))
	{
		return false;
	}

	DWORD dwWidth = m_pMiniMapRT->m_TexDesc.Width;
	DWORD dwHeight = m_pMiniMapRT->m_TexDesc.Height;
	D3DXVECTOR3 vUpVector(0.0f, 1.0f, 0.0f);
	SetViewMatrix(D3DXVECTOR3(0.0f, 70.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), vUpVector);
	SetProjMatrix(D3DX_PI * 0.25f, (float)dwWidth / (float)dwHeight, 0.1f, 1000.0f);

	return true;
}

bool GMiniMap::BeginRender(ID3D11DeviceContext*    pContext, D3DXVECTOR4 vColor )
{
	if (m_pMiniMapRT->Begin(pContext, vColor) == false)
	{
		m_pMiniMapRT->End(pContext);
		return false;
	}
	return true;
}
bool GMiniMap::EndRender(ID3D11DeviceContext*    pContext)
{
	m_pMiniMapRT->End(pContext);
	return true;
}
bool GMiniMap::Frame()
{
	return true;
}
bool GMiniMap::Render(ID3D11DeviceContext*    pContext)
{
	m_pPlaneShape->m_dxobj.g_pTextureSRV = m_pMiniMapRT->m_pSRV;
	pContext->RSSetViewports(1, &m_vp);
	m_pPlaneShape->SetMatrix(NULL, NULL, NULL);
	pContext->UpdateSubresource(m_pPlaneShape->m_dxobj.g_pConstantBuffer.Get(), 0, NULL, &m_pPlaneShape->m_cbData, 0, 0);
	m_pPlaneShape->Render(pContext);
	return true;
}
HRESULT GMiniMap::CreateResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext*    pImmediateContext, UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT GMiniMap::DeleteResource()
{
	HRESULT hr = S_OK;
	return S_OK;
}

bool GMiniMap::Release()
{
	return true;
}
GMiniMap::GMiniMap(void)
{
	m_pPlaneShape = nullptr;
}

GMiniMap::~GMiniMap(void)
{
	Release();
}
