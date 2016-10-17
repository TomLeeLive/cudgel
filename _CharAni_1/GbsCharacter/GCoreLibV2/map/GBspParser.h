#pragma once
#include "../GParser.h"
#include "GSpaceDivision.h"

class GBspParser : public GParser
{
public:
	ID3D11Device*   m_pd3dDevice;
	void			SetDevice( ID3D11Device*  pd3dDevice ) 
	{ 
		m_pd3dDevice = pd3dDevice; 
	}
public:
	bool		m_bBinary;
public:
	void		SetBinary( bool bBinary ) { m_bBinary = bBinary; };
	bool		Load( ID3D11Device*  pd3dDevice, GNode* pRootNode, TCHAR* szFileName, bool Binary = false);	
	bool		Save( GNode* pRootNode, TCHAR* szFileName, bool Binary = false);
	void		WriteBspTreeFile( GNode*  pBspTree );
	void		WriteBspNode( GNode*  pNode );
	void		ReadBspTreeFile( GNode*  pBspTree );
	void		ReadBspNode( GNode*  pNode );
	void		BinaryWriteBspNode( GNode*  pNode );
	void		BinaryReadBspNode( GNode*  pNode );

	bool		LoadTexture();
	void		SetBufferOpti(GNode* pNode, int iBuffer );
	void		SetNodePerRenderGroup(GNode* pNode);
	int			IsEqulVertexList( VertexList& CollectMap, PNCT_VERTEX* pTsVertex, int iPushCnt );
public:
	GBspParser(void);
	virtual ~GBspParser(void);
};
