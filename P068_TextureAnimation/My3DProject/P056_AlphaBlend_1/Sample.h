#pragma once
#include "TCore.h"
#include "TShape.h"
#include "TCamera.h"
#include <memory>
#define MAX_CNT 3
class Sample : public TCore
{
public:
	ComPtr<ID3D11BlendState> m_pAlphaBlend;
	ComPtr<ID3D11ShaderResourceView> m_pSRV;
	TCamera m_Camera;
	shared_ptr<TShape> m_pShape[MAX_CNT];
	TLineShape m_Line;
	D3DXMATRIX m_matWorld[MAX_CNT];  //월드
	D3DXMATRIX m_matView;//뷰
	D3DXMATRIX m_matProj;//프로젝션
	D3DXMATRIX m_vStart;
	D3DXMATRIX m_vEnd;
	D3DXMATRIX m_vColor;
	D3DXVECTOR4 m_vDir;
	float		m_fSpeed;
	bool		m_bCameraType;
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
	Sample();
	virtual ~Sample();

};