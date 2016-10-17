#pragma once
#include "GMesh.h"
#include "GAseParser.h"

typedef GData<PNCT_VERTEX>				tAseData;
typedef vector<shared_ptr<tAseData>>	tAseMeshData;

class GAseObj : public GModel
{
public:
	tAseMeshData	m_pData;
	GAseParser		m_Parser;
	vector<TMtrl>	m_Material;
public:
	bool					Init();
	bool					Frame();
	bool					Render(ID3D11DeviceContext*    pContext);
	bool					Release();
	bool					ResetResource();
	bool					Convert(ID3D11Device* pDevice);
	bool					Load(ID3D11Device* pd3dDevice, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread=false);
	void					SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	HRESULT					SetInputLayout();
	HRESULT					LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString);
	void					SetTriangleBuffer(GMesh* pMesh, tAseData* pData, TAseMesh* pAseMesh,DWORD dwMask = 0x00000000);
	int						GetMapID(TMtrl* pMtrl, int iTexMapType = ID_TBASIS_DI);
	//--------------------------------------------------------------------------------------																					
	// 에니메이션 관련 함수 																					
	//--------------------------------------------------------------------------------------
	TAnimTrack*				SetDoublyLinkedList(TAnimTrack* pCurrentTrack, TAnimTrack* pPrev);//이중 연결 리스트 구축
	bool					GetAnimationTrack(float fFrame,	vector<shared_ptr<TAnimTrack>> pTrackList,	TAnimTrack** ppStartTrack,	TAnimTrack** ppEndTrack);
	void					Interpolate(GMesh* pMesh,	D3DXMATRIX* matParent,	float fFrameTick);
	bool					InheriteCollect();
	GMesh*		SearchToCollects(T_STR	m_strParentName);
	bool		UpdateBuffer();
	bool		CombineBuffer(ID3D11Buffer* pVB, ID3D11Buffer* pIB);
	bool		Draw(ID3D11DeviceContext*    pContext, GModel* pParent);
public:
	GAseObj(void);
	virtual ~GAseObj(void);
};
