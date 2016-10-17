#pragma once
#include "GMesh.h"

class GModel : public GShape
{
public:
	vector<shared_ptr<GMesh>>		m_pMesh;
	T_STR				m_szName;
	float				m_fElapseTime;
	int					m_iStartFrame;
	int					m_iLastFrame;
	TScene				m_Scene;
	D3DXMATRIX			m_matControlWorld;
	bool				m_bSetLoadDraw;
	UINT				m_iMaxVertex;
	UINT				m_iMaxIndex;
	UINT				m_iBeginPosVB;
	UINT				m_iBeginPosIB;
public:
	GModel(void);
	virtual ~GModel(void);
};
