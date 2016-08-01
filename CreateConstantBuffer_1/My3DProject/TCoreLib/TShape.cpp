#include "TShape.h"

void   TShape::SetMatrix(D3DXMATRIX* pWorld,
	D3DXMATRIX* pView,
	D3DXMATRIX* pProj)
{
	if(pWorld == nullptr)
	{
		D3DXMatrixIdentity(&m_matWorld);
	}
	else
	{
		m_matWorld = *pWorld;
	}
	m_vPos.x = m_matWorld._41;
	m_vPos.y = m_matWorld._42;
	m_vPos.z = m_matWorld._43;

	m_vSide.x = m_matWorld._11;
	m_vSide.y = m_matWorld._12;
	m_vSide.z = m_matWorld._13;
	
	D3DXVec3Normalize(&m_vSide, &m_vSide);
	m_vUp.x = m_matWorld._21;
	m_vUp.y = m_matWorld._22;
	m_vUp.z = m_matWorld._23;
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	m_vLook.x = m_matWorld._31;
	m_vLook.y = m_matWorld._32;
	m_vLook.z = m_matWorld._33;
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	m_matView = *pView;
	m_matProj= *pProj;
	D3DXMatrixTranspose(&m_cbData.matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_cbData.matView, &m_matView);
	D3DXMatrixTranspose(&m_cbData.matProj, &m_matProj);
}
HRESULT TShape::LoadShaderResourceView(TCHAR* pTextureFile)		
{
	m_Object.g_pTexSRV.Attach(
		CreateShaderResourceView(m_pd3dDevice,
			pTextureFile));
	return S_OK;
}
HRESULT TShape::LoadShaderFile(TCHAR* pShaderFile)
{
	m_Object.g_pVertexShader.Attach(
		LoadVertexShaderFile(m_pd3dDevice,
			pShaderFile,
			m_Object.g_pVSBlob.GetAddressOf()));
	m_Object.g_pPixelShader.Attach(
		DX::LoadPixelShaderFile(m_pd3dDevice,
			pShaderFile));
	return S_OK;
}

HRESULT TShape::CreateConstantBuffer()
{
	m_Object.g_pConstantBuffer.Attach(
		CreateBuffer(g_pd3dDevice, &m_cbData,
			1, sizeof(VS_CONSTANT_BUFFER),
			D3D11_BIND_CONSTANT_BUFFER));
	return S_OK;
}
bool  TShape::Create(ID3D11Device* pd3dDevice,
	TCHAR* pShaderFile,
	TCHAR* pTextureFile)
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;
#pragma region CreateVertexBuffer
	if (FAILED(hr= CreateVertexBuffer()))
	{
		return false;
	}
#pragma endregion
#pragma region CreateIndexBuffer
	if (FAILED(hr = CreateIndexBuffer()))
	{
		return false;
	}
#pragma endregion
#pragma region g_pConstantBuffer
	if (FAILED(hr = CreateConstantBuffer()))
	{
		return false;
	}
#pragma endregion
#pragma region LoadShaderFile
	if (FAILED(hr = LoadShaderFile(pShaderFile)))
	{
		return false;
	}
#pragma endregion
#pragma region g_pInputlayout
	if (FAILED(hr = SetInputlayout()))
	{
		return false;
	}
#pragma endregion
#pragma region LoadShaderResourceView
	if (pTextureFile != nullptr)
	{
		if (FAILED(hr = LoadShaderResourceView(pTextureFile)))
		{
			return false;
		}
	}
#pragma endregion
	
	return true;
}
bool TShape::PreRender(ID3D11DeviceContext*   pContext)
{	
	ApplySS(pContext, TDxState::g_pSamplerState);

	m_Object.PreRender(pContext,
		m_Object.m_iVertexSize,
		m_Object.m_iNumIndex);
	return true;
}

bool TShape::PostRender(ID3D11DeviceContext*   pContext)
{
	pContext->UpdateSubresource(
		m_Object.g_pConstantBuffer.Get(), 0, NULL,
		&m_cbData, 0, 0);

	m_Object.PostRender(pContext,
		m_Object.m_iVertexSize,
		m_Object.m_iNumIndex);
	return true;
}
bool TShape::Render(ID3D11DeviceContext*   pContext)
{	
	PreRender(pContext);
	PostRender(pContext);
	return true;
}
TShape::TShape()
{

}


TShape::~TShape()
{
}

HRESULT TPlaneShape::CreateVertexBuffer()
{
	HRESULT hr=S_OK;
#pragma region g_pVertexBuffer
	m_VertexList.resize(4);
	m_VertexList[0] = PCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[1] = PCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[2] = PCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[3] = PCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	m_Object.m_iNumVertex = m_VertexList.size();
	m_Object.m_iVertexSize = sizeof(PCT_VERTEX);
	m_Object.g_pVertexBuffer.Attach(
		DX::CreateBuffer(g_pd3dDevice, 
			&m_VertexList.at(0),
			m_Object.m_iNumVertex, 
			m_Object.m_iVertexSize,
			D3D11_BIND_VERTEX_BUFFER));
	if (m_Object.g_pVertexBuffer == nullptr)
	{
		return E_FAIL;
	}
#pragma endregion
	return hr;
}

HRESULT TPlaneShape::CreateIndexBuffer()
{
	HRESULT hr=S_OK;

	m_IndexList.resize(6);
	m_IndexList[0] = 0;
	m_IndexList[1] = 1;
	m_IndexList[2] = 2;
	m_IndexList[3] = 0;
	m_IndexList[4] = 2;
	m_IndexList[5] = 3;
	
	m_Object.m_iNumIndex = m_IndexList.size();
	m_Object.g_pIndexBuffer.Attach(
		CreateBuffer(g_pd3dDevice, &m_IndexList.at(0),
			m_Object.m_iNumIndex, sizeof(DWORD),
			D3D11_BIND_INDEX_BUFFER		));
	if (m_Object.g_pIndexBuffer == nullptr)
	{
		return E_FAIL;
	}
#pragma endregion
	return hr;
}

HRESULT TPlaneShape::SetInputlayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_Object.g_pInputlayout.Attach(
		CreateInputlayout(g_pd3dDevice,
		layout, numElements,
		m_Object.g_pVSBlob.Get()));
	return S_OK;
}
HRESULT TDirectionLineShape::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	m_VertexList.resize(6);
	m_VertexList[0] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
		D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
	m_VertexList[1] = PC_VERTEX(D3DXVECTOR3(1.0f, 0.0f, 0.0f), 
		D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));

	m_VertexList[2] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	m_VertexList[3] = PC_VERTEX(D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));

	m_VertexList[4] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
	m_VertexList[5] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));

	m_Object.m_iNumVertex = m_VertexList.size();
	m_Object.m_iVertexSize = sizeof(PC_VERTEX);
	m_Object.g_pVertexBuffer.Attach(
		DX::CreateBuffer(g_pd3dDevice,
			&m_VertexList.at(0),
			m_Object.m_iNumVertex,
			m_Object.m_iVertexSize,
			D3D11_BIND_VERTEX_BUFFER));
	if (m_Object.g_pVertexBuffer == nullptr)
	{
		return E_FAIL;
	}
	return hr;
};
HRESULT TDirectionLineShape::CreateIndexBuffer() {
	HRESULT hr = S_OK;
	m_IndexList.resize(6);
	m_IndexList[0] = 0;
	m_IndexList[1] = 1;
	m_IndexList[2] = 2;
	m_IndexList[3] = 3;
	m_IndexList[4] = 4;
	m_IndexList[5] = 5;

	m_Object.m_iNumIndex = m_IndexList.size();
	m_Object.g_pIndexBuffer.Attach(
		CreateBuffer(g_pd3dDevice, &m_IndexList.at(0),
			m_Object.m_iNumIndex, sizeof(DWORD),
			D3D11_BIND_INDEX_BUFFER));
	if (m_Object.g_pIndexBuffer == nullptr)
	{
		return E_FAIL;
	}
	return hr;
};
HRESULT TDirectionLineShape::SetInputlayout() {
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },		
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_Object.g_pInputlayout.Attach(CreateInputlayout(g_pd3dDevice,
		layout, numElements,
		m_Object.g_pVSBlob.Get()));
	return hr;
};

HRESULT TBoxShape::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	m_VertexList.resize(24);
	m_VertexList[0] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[1] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[2] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[3] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));
	// µÞ¸é
	m_VertexList[4] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[5] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[6] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[7] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// ¿ÞÂÊ
	m_VertexList[8] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[9] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[10] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[11] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// ¿À´ÃÂÊ¸é
	m_VertexList[12] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[13] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[14] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[15] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// À­¸é
	m_VertexList[16] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[17] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[18] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[19] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// ¾Æ·§¸é
	m_VertexList[20] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[21] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[22] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[23] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	m_Object.m_iNumVertex = m_VertexList.size();
	m_Object.m_iVertexSize = sizeof(PNCT_VERTEX);
	m_Object.g_pVertexBuffer.Attach(
		DX::CreateBuffer(g_pd3dDevice,
			&m_VertexList.at(0),
			m_Object.m_iNumVertex,
			m_Object.m_iVertexSize,
			D3D11_BIND_VERTEX_BUFFER));
	if (m_Object.g_pVertexBuffer == nullptr)
	{
		return E_FAIL;
	}
	return hr;
};
HRESULT TBoxShape::CreateIndexBuffer() {
	HRESULT hr = S_OK;
	UINT iIndex = 0;
	m_IndexList.resize(36);
	m_IndexList[iIndex++] = 0; 	m_IndexList[iIndex++] = 1; 	m_IndexList[iIndex++] = 2; 	m_IndexList[iIndex++] = 0;	m_IndexList[iIndex++] = 2; 	m_IndexList[iIndex++] = 3;
	m_IndexList[iIndex++] = 4; 	m_IndexList[iIndex++] = 5; 	m_IndexList[iIndex++] = 6; 	m_IndexList[iIndex++] = 4;	m_IndexList[iIndex++] = 6; 	m_IndexList[iIndex++] = 7;
	m_IndexList[iIndex++] = 8; 	m_IndexList[iIndex++] = 9; 	m_IndexList[iIndex++] = 10; m_IndexList[iIndex++] = 8;	m_IndexList[iIndex++] = 10; m_IndexList[iIndex++] = 11;
	m_IndexList[iIndex++] = 12; m_IndexList[iIndex++] = 13; m_IndexList[iIndex++] = 14; m_IndexList[iIndex++] = 12;	m_IndexList[iIndex++] = 14; m_IndexList[iIndex++] = 15;
	m_IndexList[iIndex++] = 16; m_IndexList[iIndex++] = 17; m_IndexList[iIndex++] = 18; m_IndexList[iIndex++] = 16;	m_IndexList[iIndex++] = 18; m_IndexList[iIndex++] = 19;
	m_IndexList[iIndex++] = 20; m_IndexList[iIndex++] = 21; m_IndexList[iIndex++] = 22; m_IndexList[iIndex++] = 20;	m_IndexList[iIndex++] = 22; m_IndexList[iIndex++] = 23;

	m_Object.m_iNumIndex = m_IndexList.size();
	m_Object.g_pIndexBuffer.Attach(
		CreateBuffer(g_pd3dDevice, &m_IndexList.at(0),
			m_Object.m_iNumIndex, sizeof(DWORD),
			D3D11_BIND_INDEX_BUFFER));
	if (m_Object.g_pIndexBuffer == nullptr)
	{
		return E_FAIL;
	}
	return hr;
};
HRESULT TBoxShape::SetInputlayout() {
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,40,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_Object.g_pInputlayout.Attach(CreateInputlayout(g_pd3dDevice,
		layout, numElements,
		m_Object.g_pVSBlob.Get()));
	return hr;
};

HRESULT TLineShape::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	m_VertexList.resize(2);
	m_VertexList[0] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f));
	m_VertexList[1] = PC_VERTEX(D3DXVECTOR3(1.0f, 0.0f, 0.0f),
		D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f));

	m_Object.m_iNumVertex = m_VertexList.size();
	m_Object.m_iVertexSize = sizeof(PC_VERTEX);
	m_Object.g_pVertexBuffer.Attach(
		DX::CreateBuffer(g_pd3dDevice,
			&m_VertexList.at(0),
			m_Object.m_iNumVertex,
			m_Object.m_iVertexSize,
			D3D11_BIND_VERTEX_BUFFER));
	if (m_Object.g_pVertexBuffer == nullptr)
	{
		return E_FAIL;
	}
	return hr;
};
HRESULT TLineShape::CreateIndexBuffer() {
	HRESULT hr = S_OK;
	m_IndexList.resize(6);
	m_IndexList[0] = 0;
	m_IndexList[1] = 1;

	m_Object.m_iNumIndex = m_IndexList.size();
	m_Object.g_pIndexBuffer.Attach(
		CreateBuffer(g_pd3dDevice, &m_IndexList.at(0),
			m_Object.m_iNumIndex, sizeof(DWORD),
			D3D11_BIND_INDEX_BUFFER));
	if (m_Object.g_pIndexBuffer == nullptr)
	{
		return E_FAIL;
	}
	return hr;
};
HRESULT TLineShape::SetInputlayout() {
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_Object.g_pInputlayout.Attach(CreateInputlayout(g_pd3dDevice,
		layout, numElements,
		m_Object.g_pVSBlob.Get()));
	return hr;
};
bool  TLineShape::Draw(ID3D11DeviceContext*   pContext,
	D3DXVECTOR3 vStart,
	D3DXVECTOR3 vEnd,
	D3DXVECTOR4 vColor)
{
	m_VertexList[0].p = vStart;
	m_VertexList[0].c = vColor;
	m_VertexList[1].p = vEnd;
	m_VertexList[1].c = vColor;
	pContext->UpdateSubresource(
		m_Object.g_pVertexBuffer.Get(),
		0, 0, &m_VertexList.at(0), 0, 0);
	return Render(pContext);
}