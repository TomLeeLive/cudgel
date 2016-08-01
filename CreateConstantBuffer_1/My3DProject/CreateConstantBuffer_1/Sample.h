#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "TCore.h"
//---------------------
//구조체
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
	// 버텍스 버퍼 및 레이아웃
	//-------------------------
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer*	   m_pVertexBuffer;
	ID3D11Buffer*	   m_pIndexBuffer;
	ID3D11Buffer*	   m_pConstantBuffer;

	//-------------------------
	// 버텍스 및 픽셀 쉐이더
	//-------------------------
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;
	D3D_PRIMITIVE_TOPOLOGY m_PrimType;
	
	D3DXMATRIX	m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX   m_matProj;

public:
	//------------------------
	// 삼각형 생성 및 초기화
	//------------------------
	HRESULT CreateTriangle(D3D_PRIMITIVE_TOPOLOGY iTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//-----------------------------------------------------------
	// 정점 및 픽쉘 셰이더 로딩 및 생성
	//-----------------------------------------------------------
	HRESULT LoadShaderAndInputLayout();

	//-----------------------------------------------------------
	// 정점 버퍼 생성
	//-----------------------------------------------------------
	HRESULT CreateVertexBuffer();

	//-----------------------------------------------------------
	// 인덱스 버퍼 생성
	//-----------------------------------------------------------
	HRESULT CreateIndexBuffer();

	//-----------------------------------------------------------
	// 상수 버퍼 생성
	//-----------------------------------------------------------
	HRESULT CreateConstantBuffer();
	//-----------------------------------------------------------
	// 재정의 함수
	//-----------------------------------------------------------

	bool Init();
	bool Frame();
	bool Render();
	bool Release();

	Sample();
	virtual ~Sample();
};

