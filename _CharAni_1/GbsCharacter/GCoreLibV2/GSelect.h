#pragma once
#include "GShape.h"

struct G_RAY
{
	float			fExtent;
	D3DXVECTOR3		vOrigin;
	D3DXVECTOR3		vDirection;
};

class GSelect
{
public:
	G_RAY		m_Ray;
	D3DXVECTOR3 m_vIntersection;
	D3DXMATRIX  m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX  m_matProj;
	D3DXVECTOR3 m_vDxR;
	D3DXVECTOR3 m_vCenter;
public:
	D3DXVECTOR3		m_vSrcVex[3];
	float			m_fPickDistance;
	D3DXMATRIX		m_matWorldPick;
	D3DXVECTOR3 qvec;
	D3DXVECTOR3 tvec;
	D3DXVECTOR3 pvec;
public:
	void SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	void Update();
	bool IntersectRayToSphere(G_SPHERE* pSphere, G_RAY* pRay = NULL);

	bool ChkOBBToRay(G_BOX* pBox, G_RAY* pRay = NULL);
	bool IntersectBox(G_BOX* pBox, G_RAY* pRay = NULL);

	bool AABBtoRay(G_BOX* a, G_RAY* pRay = NULL);
	bool OBBtoRay(G_BOX* a, G_RAY* pRay = NULL);
	D3DXVECTOR3 GetIntersection();

	// 교점을 찾는다.
	bool    GetIntersection(
		D3DXVECTOR3 vStart,
		D3DXVECTOR3 vEnd,
		D3DXVECTOR3 vNormal,
		D3DXVECTOR3 v0,
		D3DXVECTOR3 v1,
		D3DXVECTOR3 v2);
	// 폴리곤안에 위치하는지 판단한다.
	bool    PointInPolygon(
		D3DXVECTOR3 vert,
		D3DXVECTOR3 faceNormal,
		D3DXVECTOR3 v0,
		D3DXVECTOR3 v1,
		D3DXVECTOR3 v2);
	bool	ChkPick(D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2);
	bool	IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
		D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
		FLOAT* t, FLOAT* u, FLOAT* v);

public:
	GSelect(void);
	virtual ~GSelect(void);
};

