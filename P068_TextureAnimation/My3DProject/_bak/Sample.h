#pragma once
#include "Tcore.h"
#include "TShape.h"
#include "TCamera.h"
#include <memory>
#define MAX_CNT 3
class Sample : public TCore
{
public:
	ComPtr<ID3D11BlendState>  m_pAlphaBlend;
	ComPtr<ID3D11ShaderResourceView>  m_pSRV[5];
	ComPtr<ID3D11PixelShader>  m_pPS[2];
	TCamera		m_Camera;
	shared_ptr<TShape> m_pShape[MAX_CNT];
	TLineShape   m_Line;
	D3DXMATRIX   m_matWorld[MAX_CNT];
	D3DXMATRIX   m_matView;
	D3DXMATRIX   m_matProj;
	D3DXVECTOR3  m_vStart;
	D3DXVECTOR3  m_vEnd;
	D3DXVECTOR4  m_vColor;
	D3DXVECTOR4  m_vDir;
	float		 m_fSpeed;
	bool			m_bCameraType;
public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();
	Sample();
	virtual ~Sample();
};

