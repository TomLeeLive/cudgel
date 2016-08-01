#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "TCore.h"
//---------------------
//����ü
//---------------------
struct P3VERTEX
{
	float fPositionX;
	float fPositionY;
	float fPositionZ;
};


class Sample : public TCore
{
public:
	//-------------------------
	// ���ؽ� ���� �� ���̾ƿ�
	//-------------------------
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer*	   m_pVertexBuffer;
	ID3D11Buffer*	   m_pIndexBuffer;
	ID3D11Buffer*	   m_pConstantBuffer;

	//-------------------------
	// ���ؽ� �� �ȼ� ���̴�
	//-------------------------
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;
	D3D_PRIMITIVE_TOPOLOGY m_PrimType;
	
	D3DXMATRIX	m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX   m_matProj;

public:
	//------------------------
	// �ﰢ�� ���� �� �ʱ�ȭ
	//------------------------
	HRESULT CreateTriangle(D3D_PRIMITIVE_TOPOLOGY iTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//-----------------------------------------------------------
	// ���� �� �Ƚ� ���̴� �ε� �� ����
	//-----------------------------------------------------------
	HRESULT LoadShaderAndInputLayout();

	//-----------------------------------------------------------
	// ���� ���� ����
	//-----------------------------------------------------------
	HRESULT CreateVertexBuffer();

	//-----------------------------------------------------------
	// �ε��� ���� ����
	//-----------------------------------------------------------
	HRESULT CreateIndexBuffer();

	//-----------------------------------------------------------
	// ��� ���� ����
	//-----------------------------------------------------------
	HRESULT CreateConstantBuffer();
	//-----------------------------------------------------------
	// ������ �Լ�
	//-----------------------------------------------------------

	bool Init();
	bool Frame();
	bool Render();
	bool Release();

	Sample();
	virtual ~Sample();
};

