#pragma once
#include "TDxHelper.h"
namespace DX
{
	class TDxState
	{
	public:
		static ID3D11RasterizerState*   g_pWireFrameRS;
		static ID3D11RasterizerState*   g_pBackSolidRS;
		static ID3D11RasterizerState*   g_pFrontSolidRS;
		static ID3D11RasterizerState*   g_pNoneSolidRS;		
		static ID3D11RasterizerState*   g_pCullSolidRS[3];

		static ID3D11DepthStencilState* g_pDepthStencilAddDSS; // 깊이 버퍼링 할성화
		static ID3D11DepthStencilState* g_pDepthStencilDisableDSS;// 비활성화
		
		static ID3D11SamplerState*		g_pSamplerState;
		static HRESULT SetState(ID3D11Device* pDevice);
		static bool Release();
	};

	static void ApplyRS(
		ID3D11DeviceContext*    pContext,
		ID3D11RasterizerState*  pRasterState)
	{
		pContext->RSSetState(pRasterState);
	}
	static void ApplyDSS(
		ID3D11DeviceContext*    pContext,
		ID3D11DepthStencilState*  pRasterState)
	{
		pContext->OMSetDepthStencilState(pRasterState, 0x01);
	}
	static void ApplySS(
		ID3D11DeviceContext*    pContext,
		ID3D11SamplerState*  pSamplerState)
	{
		pContext->PSSetSamplers(0, 1, &pSamplerState);
	}
	

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	ID3D11VertexShader*		LoadVertexShaderFile(
		ID3D11Device*  pd3dDevice,
		TCHAR* pLoadShaderFile,
		ID3DBlob** ppVSBlob);
	ID3D11PixelShader*		LoadPixelShaderFile(
		ID3D11Device*  pd3dDevice, 
		TCHAR* pLoadShaderFile,
		char* csEntry = 0);
	ID3D11GeometryShader*   LoadGeometryShaderFile(
		ID3D11Device*  pd3dDevice,
		TCHAR* pLoadShaderFile);
	ID3D11InputLayout* CreateInputlayout(
		ID3D11Device*  pd3dDevice,
		D3D11_INPUT_ELEMENT_DESC layout[],
		UINT numElements,
		ID3DBlob* pVSBlob);

	ID3D11Buffer* CreateBuffer(
		ID3D11Device*  pd3dDevice,
		void *data,
		UINT iNumCount,
		UINT iSize,
		D3D11_BIND_FLAG bindFlag,
		bool bDynamic = false);

	ID3D11ShaderResourceView* CreateShaderResourceView(
		ID3D11Device*  pd3dDevice, 
		TCHAR* szFileName);
	class TDXObject
	{
	public:
		TDXObject::TDXObject();
		TDXObject::~TDXObject();
		ComPtr<ID3D11Buffer>	g_pVertexBuffer;
		ComPtr<ID3D11Buffer>		g_pIndexBuffer;
		ComPtr<ID3D11Buffer>		g_pConstantBuffer;
		ComPtr<ID3D11VertexShader> g_pVertexShader;
		ComPtr<ID3D11PixelShader>  g_pPixelShader;
		ComPtr<ID3D11GeometryShader>  g_pGeometryShader;
		ComPtr<ID3D11InputLayout>  g_pInputlayout;
		ComPtr<ID3DBlob>			g_pVSBlob;
		ComPtr<ID3D11ShaderResourceView>   g_pTexSRV;

		UINT    m_iNumVertex;
		UINT    m_iNumIndex;
		UINT    m_iVertexSize;

		bool m_bWireFrameRender;
		int m_iPrimitiveType;
		int m_iCullMode;
	public:
		bool Frame();
		bool PreRender(ID3D11DeviceContext*    pContext, UINT VertexSize, UINT VertexCount);
		bool Render(ID3D11DeviceContext*    pContext, UINT VertexSize, UINT VertexCount);
		bool PostRender(ID3D11DeviceContext*    pContext, UINT VertexSize, UINT VertexCount);
		void  Release();
	};
	class TDxRT
	{
	public:
		ID3D11RenderTargetView*  m_pRenderTargetView;
		ID3D11DepthStencilView*  m_pDSV;
		HRESULT					 CreateDSV(ID3D11Device*  pd3dDevice,
			UINT iWidth, UINT iHeight);
		void					 Release()
		{			
			if (m_pRenderTargetView)
				m_pRenderTargetView->Release();
			if (m_pDSV)
				m_pDSV->Release();
		};
	};
}
