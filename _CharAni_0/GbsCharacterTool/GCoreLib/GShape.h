#pragma once

#include <tchar.h>
#include <directxMath.h>
#include "GDxHelperEx.h"

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct G_PLANE
{
	float	fA, fB, fC, fD;
	bool	CreatePlane(D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR3 v2);
	bool	CreatePlane(D3DXVECTOR3 vNormal, D3DXVECTOR3 v0);
	void	Normalize()
	{
		float fMag = sqrt(fA*fA + fB*fB + fC*fC);
		fA = fA / fMag;
		fB = fB / fMag;
		fC = fC / fMag;
		fD = fD / fMag;
	}

};
struct G_SPHERE
{
	D3DXVECTOR3		vCenter;
	float			fRadius;
};
struct G_BOX
{
	// Common
	D3DXVECTOR3		vCenter;
	D3DXVECTOR3		vPos[8];
	// AABB
	D3DXVECTOR3		vMax;
	D3DXVECTOR3		vMin;
	// OBB
	D3DXVECTOR3		vAxis[3];
	float			fExtent[3];
};
//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct PCT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR4 c;
	D3DXVECTOR2 t;
	bool operator == (const PCT_VERTEX & Vertex)
	{
		if (p == Vertex.p  && c == Vertex.c    && t == Vertex.t)
		{
			return true;
		}
		return  false;
	}
	PCT_VERTEX() {}
	PCT_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt)
	{
		p = vp, c = vc, t = vt;
	}
};
struct PCT2_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR4 c;
	D3DXVECTOR2 t0;
	D3DXVECTOR2 t1;
	bool operator == (const PCT2_VERTEX & Vertex)
	{
		if (p == Vertex.p  && c == Vertex.c    && t0 == Vertex.t0 && t1 == Vertex.t1)
		{
			return true;
		}
		return  false;
	}
	PCT2_VERTEX() {}
	PCT2_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt0,
		D3DXVECTOR2     vt1)
	{
		p = vp, c = vc, t0 = vt0, t1 = vt1;
	}
};
struct PC_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR4 c;
	bool operator == (const PC_VERTEX & Vertex)
	{
		if (p == Vertex.p  && c == Vertex.c)
		{
			return true;
		}
		return  false;
	}
	PC_VERTEX() {}
	PC_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR4		vc)
	{
		p = vp, c = vc;
	}
};
struct PNC_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR4 c;
	bool operator == (const PNC_VERTEX & Vertex)
	{
		if (p == Vertex.p  && n == Vertex.n && 	c == Vertex.c)
		{
			return true;
		}
		return  false;
	}
	PNC_VERTEX() {}
	PNC_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR3		vn,
		D3DXVECTOR4		vc)
	{
		p = vp, n = vn, c = vc;
	}
};
struct PNCT_VERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR3		n;
	D3DXVECTOR4		c;
	D3DXVECTOR2     t;
	bool operator == (const PNCT_VERTEX & Vertex)
	{
		if (p == Vertex.p  && n == Vertex.n && 	c == Vertex.c  &&	t == Vertex.t)
		{
			return true;
		}
		return  false;
	}
	PNCT_VERTEX() {}
	PNCT_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR3		vn,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt)
	{
		p = vp, n = vn, c = vc, t = vt;
	}
};
struct VS_CONSTANT_BUFFER
{
	D3DXMATRIX matWorld;// c0						
	D3DXMATRIX matView;	// c4						
	D3DXMATRIX matProj;	// c8						
	D3DXVECTOR4 Color; // 12
};
class GShape
{
public:
	ID3D11Device*				m_pd3dDevice;
	ID3D11DeviceContext*		m_pContext;
	DX::GDxObject				m_Object;
	VS_CONSTANT_BUFFER			m_cbData;
	D3DXVECTOR3					m_vCenterPos;
	D3DXMATRIX					m_matWorld;
	D3DXMATRIX					m_matView;
	D3DXMATRIX					m_matProj;
	vector<DWORD>				m_IndexList;
	//--------------------------------------------------------------------------------------
	// Collision Data
	//--------------------------------------------------------------------------------------
	G_BOX                       m_Box;
	G_SPHERE                    m_Sphere;
	G_PLANE                     m_Plane;
public:
	ID3D11Buffer*				GetVB();
	ID3D11Buffer*				GetIB();
	virtual void				SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	virtual void				SetAmbientColor(float fR, float fG, float fB, float fA);
	virtual bool				Convert(ID3D11Device* pDevice);
	virtual bool				Load(ID3D11Device* pDevice, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile);
	virtual HRESULT				LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile   );
	virtual HRESULT				SetInputLayout();
	virtual HRESULT				CreateVertexBuffer();
	virtual HRESULT				CreateIndexBuffer();
	virtual HRESULT				CreateConstantBuffer();
	virtual HRESULT				LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString);
	virtual HRESULT				CreateResource();
	virtual HRESULT				DeleteResource();
	virtual bool				Create(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile, const TCHAR* pLoadTextureString =0 );
	virtual bool				Init();
	virtual bool				Frame();
	virtual bool				PreRender(ID3D11DeviceContext* pContext);
	virtual bool				Render(ID3D11DeviceContext*		pContext);
	virtual bool				PostRender(ID3D11DeviceContext* pContext);
	virtual bool				Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, D3DXVECTOR4 dwColor);
	virtual bool				Release();
public:
	GShape(void);
	virtual ~GShape(void);
};

class GLineShape : public GShape
{
public:
	vector<PC_VERTEX>					m_VertexList;
public:
	HRESULT				SetInputLayout();
	HRESULT				CreateVertexBuffer();
	HRESULT				CreateIndexBuffer();
	HRESULT				CreateResource();
	bool				Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, D3DXVECTOR4 dwColor);
public:
	GLineShape(void);
	virtual ~GLineShape(void);
};

class GDirectionLineShape : public GShape
{
public:
	vector<PC_VERTEX>					m_VertexList;
public:
	HRESULT			SetInputLayout();
	HRESULT			CreateVertexBuffer();
	HRESULT			CreateIndexBuffer();
	HRESULT			CreateResource();
public:
	GDirectionLineShape(void);
	virtual ~GDirectionLineShape(void);
};

class GBoxShape : public GShape
{
public:
	vector<PNCT_VERTEX>		m_VertexList;
public:
	HRESULT			SetInputLayout();
	HRESULT			CreateVertexBuffer();
	HRESULT			CreateIndexBuffer();
	HRESULT			CreateResource();
public:
	GBoxShape(void);
	virtual ~GBoxShape(void);
};


class GPlaneShape : public GShape
{
public:
	vector<PCT_VERTEX>		m_VertexList;
public:
	HRESULT			SetInputLayout();
	HRESULT			CreateVertexBuffer();
	HRESULT			CreateIndexBuffer();
	HRESULT			CreateResource();
public:
	GPlaneShape(void);
	virtual ~GPlaneShape(void);
};


class GCylinder : public GShape
{
public:
	vector<PNCT_VERTEX>		m_VertexList;
public:
	HRESULT			SetInputLayout();
	HRESULT			CreateVertexBuffer();
	HRESULT			CreateIndexBuffer();
	HRESULT			CreateResource();

public:

	struct Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v) {}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	MeshData meshData;

	float bottomRadius;
	float topRadius;
	float height;
	UINT sliceCount;
	UINT stackCount;

public:
	void			CreateCylinder();
	void			BuildCylinderTopCap();
	void			BuildCylinderBottomCap();
public:
	GCylinder(void);
	virtual ~GCylinder(void);
};