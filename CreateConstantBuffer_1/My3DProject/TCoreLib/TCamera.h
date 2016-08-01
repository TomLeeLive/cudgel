#pragma once
#include "TStd.h"
class TCamera
{
public:
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
	D3DXVECTOR3  m_vPos;
	D3DXVECTOR3 m_vTarget;
	D3DXVECTOR3 m_vUp;

	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vSide;

	float      m_fRoll;
	float      m_fYaw;
	float      m_fPitch;
	float	   m_fRadius;
public:
	bool  Create(ID3D11Device * pDevice);
	void  Update(D3DXVECTOR4 vDir);
	void  Frame(D3DXVECTOR4 vDir);
	void  UpdateVector();
	void  MoveLook(float fValue);
	D3DXMATRIX SetViewMatrix(D3DXVECTOR3 vPos, D3DXVECTOR3 vAt, D3DXVECTOR3 vUp=D3DXVECTOR3(0,1,0) );
	D3DXMATRIX SetProjMatrix(FLOAT fFov, FLOAT fAsepect, FLOAT fNear, FLOAT fFar);
public:
	TCamera();
	virtual ~TCamera();
};

