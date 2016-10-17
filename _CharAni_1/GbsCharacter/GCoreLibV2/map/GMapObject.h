#pragma once
#include "../GBaseObj.h"
#include "../GBoxObj.h"

#define NUM_OBJECTS 1000

class GMapObject : public GBaseObj
{
public:
	D3DXMATRIX				m_matWorld;	
	D3DXVECTOR3			m_vPosition;
	D3DXVECTOR4			m_vColor;
	GBoxObj*					m_pBoxObj;
public:
	void				Update();	
	void				SetPosition( D3DXVECTOR3 vPos );
public:
	GMapObject();
	~GMapObject();
};