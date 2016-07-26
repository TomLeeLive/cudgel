#include "TCamera.h"

void  TCamera::UpdateVector()
{
	m_vLook.x = m_matView._13;
	m_vLook.y = m_matView._23;
	m_vLook.z = m_matView._33;
	m_vSide.x = m_matView._11;
	m_vSide.y = m_matView._21;
	m_vSide.z = m_matView._31;
	m_vUp.x = m_matView._12;
	m_vUp.y = m_matView._22;
	m_vUp.z = m_matView._32;
	
}
bool  TCamera::Create(ID3D11Device * pDevice)
{
	m_vPos = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_vTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &m_vUp);

	UpdateVector();

	float m_fAspet = 800.0f / 600.0f;
	float m_fFov = D3DX_PI * 0.25f;
	float fNear = 1.0f;
	float fFar = 1000.0f;
	D3DXMatrixPerspectiveFovLH(&m_matProj,
		m_fFov, m_fAspet, fNear, fFar);
	return true;
}
void   TCamera::MoveLook(float fValue)
{
	m_vPos += m_vLook * fValue;
}
void   TCamera::Frame(D3DXVECTOR4 vDir)
{	
	D3DXQUATERNION qRot;
	D3DXQuaternionRotationYawPitchRoll(&qRot,
		vDir.y, vDir.x, vDir.z);	

	m_fRadius += vDir.w;
	m_vPos += m_vLook*vDir.w;

	D3DXMATRIX matRot;
	// 사원수에 위치를 넣어서 행렬로 변환한다.
	D3DXMatrixAffineTransformation(
		&matRot, 1.0f, NULL, &qRot, &m_vPos);

	D3DXMatrixInverse(&m_matView, NULL, &matRot);	

	UpdateVector();
}
void   TCamera::Update(D3DXVECTOR4 vDir)
{
	m_fRadius += vDir.w;

	D3DXMATRIX matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		vDir.y, vDir.x, vDir.z);

	D3DXVECTOR3 vLocalLook(0, 0, 1);
	D3DXVECTOR3 vUpLook(0, 1, 0);
	D3DXVECTOR3 vWorldLook;
	D3DXVECTOR3 vWorldUp;

	D3DXVec3TransformCoord(&vWorldLook, &vLocalLook, &matRot);
	D3DXVec3TransformCoord(&vWorldUp, &vUpLook, &matRot);

	m_vPos = m_vTarget + (-vWorldLook*m_fRadius);

	SetViewMatrix(m_vPos, m_vTarget, vWorldUp);
}
D3DXMATRIX TCamera::SetViewMatrix(D3DXVECTOR3 vPos, 
	D3DXVECTOR3 vAt, D3DXVECTOR3 vUp)
{
	m_vTarget = vAt;
	m_vPos = vPos;
	m_vUp = vUp;

	D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &m_vUp);

	UpdateVector();

	return m_matView;
};
D3DXMATRIX TCamera::SetProjMatrix(FLOAT fFov, FLOAT fAsepect, FLOAT fNear, FLOAT fFar)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj,
		fFov, fAsepect, fNear, fFar);
	return m_matProj;
};
TCamera::TCamera()
{
	m_fRadius = 10.0f;
}


TCamera::~TCamera()
{
}
