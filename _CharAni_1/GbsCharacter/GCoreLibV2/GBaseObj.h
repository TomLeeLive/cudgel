#pragma once
#include "GFrustum.h"

class GBaseObj
{
public:
	int					m_iIndex;
	G_BOX				m_tBox;
	D3DXMATRIX			m_matWorld;
	D3DXVECTOR3			m_vPosition;
	D3DXVECTOR4			m_vColor;
public:
	virtual void		Update() {};
public:
	 GBaseObj(void);
	 virtual ~GBaseObj(void);
};
