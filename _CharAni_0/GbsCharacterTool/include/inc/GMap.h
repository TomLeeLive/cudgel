#pragma once
#include "GShape.h"
using namespace DX;

#define RANDOM(n)			(rand() % (n))
#define RANDOMSCOPE(s,e)	(rand() %(e-s))+s;
#define RANDOMIZE()			srand((unsigned)time(NULL))

struct GMapDesc
{
	int				iNumCols;
	int				iNumRows;
	float			fSellDistance;
	T_STR			strTextureFile;
	T_STR			strShaderFile;
};

class GMap : public GShape
{
public:
	PNCT_VERTEX*			m_pvHeightMap;
	DWORD					m_dwShaderIndex;
	DWORD*					m_pdwIndexArray;
	int						m_iNumVertices;
	int						m_iNumFace;
	int						m_iTexIndex;
	int						m_iNumCols;
	int						m_iNumRows;
	int						m_iNumSellCols;
	int						m_iNumSellRows;
	float					m_fSellDistance;
	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProj;
	D3DXMATRIX				m_matNormal;
	D3DXVECTOR3				m_vLightDir;
	int*					m_pNormalLookupTable;
	D3DXVECTOR3*			m_pFaceNormals;
public:
	virtual HRESULT		SetInputLayout();
	virtual bool		ReLoadVBuffer();
	virtual bool		ReLoadIBuffer();
	virtual void		CalcVertexColor(D3DXVECTOR3 vLightDir);
	virtual float		GetHeightOfVertex(UINT Index);
	virtual D3DXVECTOR3 GetNormalOfVertex(UINT Index);
	virtual D3DXVECTOR4 GetColorOfVertex(UINT Index);
	virtual HRESULT		CreateVertexBuffer();
	virtual HRESULT		CreateIndexBuffer();
	virtual bool		CreateVertexList();
	virtual bool		CreateIndexList();
	virtual bool		CreateMap(GMapDesc& MapDesc);
	virtual bool		Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext*  pImmediateContext);
	virtual bool		Load(GMapDesc& MapDesc);
	virtual bool		Frame();
	virtual bool		Render(ID3D11DeviceContext*  pContext);
	virtual bool		Release();
	//--------------------------------------------------------------------------------------
	// 변경된 클라이언트 영역를 재설정을 위한 소멸 및 생성
	//--------------------------------------------------------------------------------------
	virtual HRESULT		CreateResource();
	virtual HRESULT		DeleteResource();
	//--------------------------------------------------------------------------------------
	// 정점 노말 연산를 위한 초기화 함수 및 페이스 노말 계산
	//--------------------------------------------------------------------------------------	
	void				InitFaceNormals();
	virtual void		CalcFaceNormals();
	D3DXVECTOR3			ComputeFaceNormal(DWORD dwIndex0, DWORD dwIndex1, DWORD dwIndex2);
	//--------------------------------------------------------------------------------------
	// 정점 노말 룩업 테이블 생성 및 계산
	//--------------------------------------------------------------------------------------	
	void				GenNormalLookupTable();
	virtual void		CalcPerVertexNormalsFastLookup();

	DXGI_FORMAT			MAKE_TYPELESS(DXGI_FORMAT format);
	DXGI_FORMAT			MAKE_SRGB(DXGI_FORMAT format);
public:
	GMap(void);
	virtual ~GMap(void);
};
