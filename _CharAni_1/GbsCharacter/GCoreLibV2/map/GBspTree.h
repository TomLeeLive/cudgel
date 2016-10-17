#pragma once
#include "GBspParser.h"
#include "GBspRender.h"

class GBspTree : public GSpaceDivision
{
public:		
	GBspParser			m_BspParser;
	GBspRender			m_BspRender;

	ID3D11Device*		GetDevice(){ return m_BspRender.m_pd3dDevice; }

	GPolygon			*m_pCollisionPoly;	// Collision Polygon Data
	GNode				*m_pCollisionNode;	// Collision Node
	GNode				*m_pRootNode;		// BSP tree root node

	queue< GNode* >		m_QuadTreeQueue;
	vector< GNode* >	m_DrawNodeList;
	vector< GBaseObj* >	m_DrawObjList;

	D3DXVECTOR3			m_vCurrentPosition;

public:		
	void				SetCurrentPosition( D3DXVECTOR3 vPos ) { m_vCurrentPosition = vPos; };
	bool				Build(ID3D11Device*   pd3dDevice, GPolygon* pPolyList,  TCHAR*	pLoadShaderString );
	bool				Load( ID3D11Device*  pd3dDevice, TCHAR* szFileName, bool Binary = false);
	bool				Init();
	bool				Frame();			
	bool				Release();	
	bool				Render(ID3D11DeviceContext*		pContext);
	
	void				BuildBSPTree( GNode* , GPolygon*  );
	void				SetPolygon( GPolygon* pSplitter, GPolygon* pPoly,vector<PNCT_VERTEX>&	VertexList);
	void				SetMatrix( D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj );
	GPolygon*			SelectBestSplitter( GPolygon* pPolyList );
	G_POSITION			ClassifyPoint( GPolygon* pPlane, D3DXVECTOR3* pPos );
	G_POSITION			ClassifyPoly( GPolygon* pPlane, GPolygon* pPoly );
	void				SplitterPolygon(	GPolygon* pPoly, 
												GPolygon* pPlane, 
												GPolygon* pFrontSplitter,
												GPolygon* pBackSplitter );
	bool				GetIntersection(	D3DXVECTOR3& vStart, 	
												D3DXVECTOR3& vEnd , 
												D3DXVECTOR3& vPlaneVertex, 
												D3DXVECTOR3& vPlaneNormal, 
												D3DXVECTOR3& vIntersection,	float	&fPercentage );	

	GPolygon*			GetCollisionPoly( GPolygon* pPoly );
	void				ReleaseBsp(GNode *pBspNode);
	void				ReleaseNode(GNode *pBspNode);
	int				AddObject( GBaseObj* pObj );
	GNode*			FindNode( GNode* pNode,GBaseObj* pObj );
	int				CheckRect( GNode* pNode, GBaseObj* pObj );

	void			VisibleNode( GNode* pNode );
public:
	GBspTree(void);
	virtual ~GBspTree(void);

#ifdef _DEBUG
public:
	bool				m_bDebugBspView;
	void				SetDebugBspView( bool bView = true ) { m_bDebugBspView = bView; };
	bool				GetDebugBspView() { return m_bDebugBspView; };
#endif
};
