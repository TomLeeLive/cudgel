#include "Sample.h"
#include <stdio.h>


//////////////////////////////////////////////
// DX 매소드 호출에 대한 반환값 처리 매크로
//////////////////////////////////////////////
#ifndef V
#define V(x)       { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x){ hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

struct VS_CONSTANT_BUFFER
{
	D3DXVECTOR4 vColor;	// c0
						// c1
	float fC5_X;
	float fTime;
	float fC5_Z;
	float fC5_W;
};

HRESULT Sample::LoadShaderAndInputLayout()
{
	HRESULT hr = S_OK;
	// Create the effect
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	//WCHAR str[MAX_PATH];   
	ID3DBlob* pVSBuf = NULL;

#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pBufferErrors = NULL;
	if (FAILED(hr = D3DX11CompileFromFile(L"HLSLwithoutFX.vsh", NULL, NULL, "VS", "vs_5_0", dwShaderFlags, NULL, NULL, &pVSBuf, &pBufferErrors, NULL)))
	{
		TCHAR pMessage[500];
		mbstowcs(pMessage, (CHAR*)pBufferErrors->GetBufferPointer(), 500);
		MessageBox(NULL, pMessage, _T("ERROR"), MB_OK);
		return hr;
	}
	V_RETURN(m_pd3dDevice->CreateVertexShader((DWORD*)pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVS));

	// Compile the PS from the file
	ID3DBlob* pPSBuf = NULL;
	V_RETURN(D3DX11CompileFromFile(L"HLSLwithoutFX.psh", NULL, NULL, "main", "ps_5_0", dwShaderFlags, NULL, NULL, &pPSBuf, NULL, NULL));
	V_RETURN(m_pd3dDevice->CreatePixelShader((DWORD*)pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPS));

	// Create our vertex input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	V_RETURN(m_pd3dDevice->CreateInputLayout(layout, 1, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout));
	//V_RETURN( GetDevice()->CreateInputLayout( layout, 1, ( DWORD* )g_VS, dwSize&m_pVertexLayout ) );	
	SAFE_RELEASE(pVSBuf);
	SAFE_RELEASE(pPSBuf);
	return hr;
}
HRESULT Sample::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	// Create vertex buffer
	P3VERTEX vertices[] =
	{
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
	};

	UINT numVertices = sizeof(vertices) / sizeof(vertices[0]);

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(P3VERTEX) *numVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	CD3D11_BUFFER_DESC cbc(sizeof(P3VERTEX) * 4, D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	V_RETURN(m_pd3dDevice->CreateBuffer(&cbc, &InitData, &m_pVertexBuffer));
	return hr;
}
HRESULT Sample::CreateIndexBuffer()
{
	HRESULT hr = S_OK;


	WORD indices[] =
	{
		0,1,2,
		0,2,3,
	};

	UINT iNumIndex = sizeof(indices) / sizeof(indices[0]);

	// Create an Index Buffer
	D3D11_BUFFER_DESC ibDesc;
	ibDesc.ByteWidth = iNumIndex * sizeof(WORD);
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ibInitData;
	ZeroMemory(&ibInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	ibInitData.pSysMem = indices;
	V_RETURN(m_pd3dDevice->CreateBuffer(&ibDesc, &ibInitData, &m_pIndexBuffer));
	return hr;
}
HRESULT Sample::CreateConstantBuffer()
{
	HRESULT hr;
	// Create a constant buffer
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	V_RETURN(hr = m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &m_pConstantBuffer));
	return hr;
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT Sample::CreateTriangle(D3D11_PRIMITIVE_TOPOLOGY iTopology)
{
	HRESULT hr = S_OK;
	//  정점 쉐이더 및 픽셀 쉐이더를 로딩 및 생성한다.
	V_RETURN(CreateVertexBuffer());
	V_RETURN(CreateIndexBuffer());
	V_RETURN(LoadShaderAndInputLayout());

	// Set primitive topology
	GetContext()->IASetPrimitiveTopology(iTopology);

	return hr;
}
bool Sample::Init()
{
	if (FAILED(CreateTriangle()))
	{
		MessageBox(0, _T("CreateTrangle  실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		MessageBox(0, _T("CreateConstantBuffer  실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	// 월드 행렬 세팅
	D3DXMatrixIdentity(&m_matWorld);

	// 뷰 행렬 세팅
	D3DXMatrixLookAtLH(&m_matView, &D3DXVECTOR3(0.0f, 0.0f, -2.0f),
		&D3DXVECTOR3(0.0f, 0.0f, 2.0f),
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	// 투영 행렬 세팅
	D3DXMatrixPerspectiveFovLH(&m_matProj, (float)D3DX_PI * 0.5f,
		m_dwWidth / (FLOAT)m_dwHeight,
		0.1f,
		100.0f);


	// 이벤트 타이머 작동시작
	//m_Timer.Start();
	m_Timer.Init();
	return true;
}
bool Sample::Frame()
{
	float fTime = m_Timer.m_fAccumulation;//.GetElapsedTime();
	float fBoundedTime = cosf(fTime)*0.5f + 0.5f;

	// 상수버퍼를 갱신한다.
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	GetContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD,
		0, &MappedResource);
	VS_CONSTANT_BUFFER* pConstData = (VS_CONSTANT_BUFFER*)MappedResource.pData;
	pConstData->vColor = D3DXVECTOR4(cosf(fTime), sinf(fTime), 1.0f - cosf(fTime), 1.0f);
	pConstData->fTime = fBoundedTime;
	GetContext()->Unmap(m_pConstantBuffer, 0);
	return true;
}
bool Sample::Render()
{
	GetContext()->VSSetShader(m_pVS, NULL, 0);
	GetContext()->PSSetShader(m_pPS, NULL, 0);
	GetContext()->GSSetShader(NULL, NULL, 0);
	GetContext()->IASetInputLayout(m_pVertexLayout);

	UINT stride = sizeof(P3VERTEX);
	UINT offset = 0;
	GetContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	GetContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	GetContext()->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	GetContext()->IASetPrimitiveTopology(m_PrimType);
	GetContext()->DrawIndexed(6, 0, 0);
	return true;
}
bool Sample::Release()
{
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);

	return true;
}
Sample::Sample(void)
{
	m_pVertexLayout = NULL;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_pConstantBuffer = NULL;
	m_pVS = NULL;
	m_pPS = NULL;
	m_PrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

Sample::~Sample(void)
{
}
TCORE_RUN(_T("Sample Lib"), 800, 600)