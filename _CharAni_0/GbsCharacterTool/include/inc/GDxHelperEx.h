#pragma once
#include "GTextureMgr.h"
#include "GDxState.h"

namespace DX
{
	HRESULT   CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	ID3D11VertexShader* LoadVertexShaderFile(ID3D11Device*  pd3dDevice, const void* pLoadShaderFile, ID3DBlob** ppBlobOut=nullptr, char *pFuntionName=0, bool bBinary = false);
	ID3D11PixelShader*  LoadPixelShaderFile(ID3D11Device*  pd3dDevice, const void* pLoadShaderFile, char *pFuntionName = 0, bool bBinary = false, ID3DBlob** pRetBlob = nullptr);
	ID3D11GeometryShader*  LoadGeometryShaderFile(ID3D11Device*  pd3dDevice, const void* pLoadShaderFile, ID3DBlob** ppBlobOut=nullptr, char *pFuntionName = 0, bool bBinary = false);

	ID3D11InputLayout* CreateInputlayout(ID3D11Device*  pd3dDevice, DWORD dwSize, LPCVOID lpData, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements);
	ID3D11Buffer* CreateVertexBuffer(ID3D11Device*  pd3dDevice, void *vertices, UINT iNumVertex, UINT iVertexSize, bool bDynamic = false);
	ID3D11Buffer* CreateIndexBuffer(ID3D11Device*  pd3dDevice, void *indices, UINT iNumIndex, UINT iSize, bool bDynamic = false);
	ID3D11Buffer* CreateConstantBuffer(ID3D11Device*  pd3dDevice, void *data, UINT iNumIndex, UINT iSize, bool bDynamic = false);

	ID3D11ShaderResourceView*	CreateShaderResourceView(ID3D11Device* pDevice, const TCHAR* strFilePath);
	ID3D11DepthStencilView* CreateDepthStencilView(ID3D11Device* pDevice, DWORD dwWidth, DWORD dwHeight);

	class GDxObject
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer>	g_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	g_pIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	g_pConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> g_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>  g_pPixelShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>  g_pGeometryShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  g_pInputlayout;
		Microsoft::WRL::ComPtr<ID3DBlob>			g_pVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob>			g_pGSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob>			g_pPSBlob;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_pTextureSRV;	
		UINT					m_iPrimitiveType;
		UINT					m_iCullMode;
		UINT					m_iSamplerMode;
		UINT					m_iNumVertex;
		UINT					m_iNumIndex;
		UINT					m_iVertexSize;
		D3D11_BOX				m_BoxVB;
		D3D11_BOX				m_BoxIB;
		UINT					m_iBeginVB;
		UINT					m_iBeginIB;
	public:
		void PreRender(ID3D11DeviceContext*    pContext, UINT iVertexSize = 0);
		void PostRender(ID3D11DeviceContext*    pContext,  UINT iCount = 0);
		bool Render(ID3D11DeviceContext*    pContext, UINT iVertexSize=0, UINT iCount=0);
	public:
		GDxObject::GDxObject();
		GDxObject::~GDxObject();
	};

	class GDxRT
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;	
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
		D3D11_DEPTH_STENCIL_VIEW_DESC			m_DepthStencilDesc;
		D3D11_TEXTURE2D_DESC					m_TexDesc;
		D3D11_VIEWPORT			m_vp;
		D3DXMATRIX				m_matProj;

		// Store the old render targets
		D3D11_VIEWPORT			m_vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
		UINT					m_nViewPorts;		
		ID3D11RenderTargetView* m_pOldRTV;
		ID3D11DepthStencilView* m_pOldDSV;
	public:
		HRESULT	Create(ID3D11Device* pd3dDevice, FLOAT Width=1024, FLOAT Height=1024);
		void	Set(ID3D11Device* pDevice, FLOAT TopLeftX, FLOAT TopLeftY, FLOAT Width, FLOAT Height,
					FLOAT MinDepth=0,	FLOAT MaxDepth=1);
		HRESULT UpdateDepthStencilView(ID3D11Device* pDevice, UINT dwWidth, UINT dwHeight);
		void	Apply(	ID3D11DeviceContext*    pImmediateContext,
							ID3D11RenderTargetView* pRTV = NULL,
							ID3D11DepthStencilView* pDepthStencilView = NULL,
							bool bDepthClear=true, bool bStencilClear=true);
		bool	Begin(ID3D11DeviceContext*    pImmediateContext, D3DXVECTOR4 vColor,
			bool bTargetClear=true, bool bDepthClear = true, bool bStencilClear = true);
		bool	Clear(ID3D11DeviceContext*    pImmediateContext, D3DXVECTOR4 vColor,
			bool bTargetClear = true, bool bDepthClear = true, bool bStencilClear = true);
		bool	End(ID3D11DeviceContext*    pImmediateContext);
		GDxRT();
		~GDxRT() {};
	};
}
