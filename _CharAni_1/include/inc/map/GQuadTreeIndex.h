#pragma once
#include "GQuadTree.h"
#include "GMap.h"
typedef vector<DWORD>	DWORD_VECTOR;
class GStaticData
{
public:
	DWORD					dwLevel;
	vector<DWORD_VECTOR>	IndexList;
	vector<ComPtr<ID3D11Buffer>>	IndexBufferList;
public:
	GStaticData();
	~GStaticData();
};

class GQuadTreeIndex : public GQuadTree
{
public:
	GMap*					m_pMap;
	PNCT_VERTEX*			m_pVertexList;
	DWORD*					m_IndexList;
	vector<GStaticData*>	m_pdwLodIndexArray;
	int						m_iNumFace;
	int						m_iMinSplitDistance;	// 최소 분할 크기( 정점간 간격 )
	DWORD					m_dwWidth;
	DWORD					m_dwHeight;
	bool					m_bDynamicUpdateIB;
	bool					m_bUsedCrossTypeIndex;
public:	
	bool					Build(	GMap* pMap,	DWORD dwWidth, DWORD dwHeight);	
	GNode*					CreateNode( GNode* pParentNode, DWORD dwTL,DWORD dwTR,DWORD dwBL, DWORD dwBR );
	int						UpdateIndexList(GNode* pNode, DWORD dwCurentIndex, DWORD dwLod  );
	D3DXVECTOR2				GetHeightFromNode(	DWORD dwTL, DWORD dwTR, DWORD dwBL, DWORD dwBR );
	void					ComputeBoungigBox( GNode* pNode );
	void					UpdateBoundingBox(GNode* pNode);
	bool					SubDivide(GNode* pNode);
	bool					Frame();
	bool					Release();	
	bool					Render(ID3D11DeviceContext*    pContext);
	bool					ComputeStaticLodIndex( int iSize );
	int						UpdateStaticIndexList( GNode* pNode, DWORD dwCurentIndex, DWORD dwLod  );
	DWORD					GetIndex( DWORD dwIndex, DWORD dwTL, DWORD dwTR, DWORD dwBL, DWORD dwBR, DWORD dwLod  );
	int						SetLodIndexBuffer(	GNode* pNode, 
												DWORD& dwCurentIndex, 										
												DWORD dwA, DWORD dwB, DWORD dwC,
												DWORD dwType );
	HRESULT			CreateIndexBuffer( GStaticData*  pData );
	DWORD			GetIndexCounter( DWORD dwData, DWORD dwQuadSize );
	HRESULT			CreateVertexBuffer(GNode* pNode, DWORD dwCount,PNCT_VERTEX* pvHeightMap);
	bool			CreateVertexData(GNode* pNode);
	void			UpdateObject(GNode* pNode);
public:
	GQuadTreeIndex(void);
	virtual ~GQuadTreeIndex(void);
};
