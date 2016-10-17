#pragma once
#include "GNode.h"
#include "../GModel.h"
#include "../GBaseObj.h"
#include "../GCamera.h"
#include "../GTextureMgr.h"
#include <queue>

typedef vector <PNCT_VERTEX>			VertexList;
typedef VertexList::iterator			VertexListItor;

class GSpaceDivision
{
public:
	GCamera*		m_pCamera;
	void			Update( GCamera* pCamera ) { m_pCamera = pCamera; }
public:
	TCHAR			m_szTexturePath[MAX_PATH];
	int				CalculateBox( G_BOX *tBox, GPolygon *Polylist);
public:
	GSpaceDivision(void);
	virtual ~GSpaceDivision(void);
};

