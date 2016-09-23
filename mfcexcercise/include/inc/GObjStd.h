#pragma once
#include "GTemplate.h"
// Sub texture types
#define ID_GBASIS_AM 0   // ambient
#define ID_GBASIS_DI 1   // diffuse
#define ID_GBASIS_SP 2   // specular
#define ID_GBASIS_SH 3   // shininesNs
#define ID_GBASIS_SS 4   // shininess strength
#define ID_GBASIS_SI 5   // self-illumination
#define ID_GBASIS_OP 6   // opacity
#define ID_GBASIS_FI 7   // filter color
#define ID_GBASIS_BU 8   // bump 
#define ID_GBASIS_RL 9   // reflection
#define ID_GBASIS_RR 10  // refraction 
#define ID_GBASIS_DP 11  // displacement
#define ID_GBASIS_GN 12  // Generic
#define ID_GBASIS_EV 13  // EnvMap

using namespace GBASIS;

const enum OBJECTCLASSTYPE {
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};

const enum FILETYPE {
	NULLFILE = 0,
	ASEFILE,
	TBSFILE,
	SKMFILE,
	MATFILE,
};


struct TScene
{
	int			iFirstFrame;
	int			iLastFrame;
	int			iFrameSpeed;
	int			iTickPerFrame;
};

struct GTextexMap
{
	// 오브젝트 텍스쳐 관리자에서 매터리얼의 텍스쳐파일명을 보고 DX텍스쳐 생성 후 참조인덱스를 저장한다.
	DWORD		m_dwIndex;
	// 텍스쳐 맵 타입인덱스
	DWORD		m_dwType;
	// 맵 이름
	T_STR		m_strName;
	// 맵 클래스 이름
	T_STR		m_strClassName;
	// 맵 텍스쳐 이름.
	T_STR		m_strTextureName;
	GTextexMap() : m_dwType(1), m_dwIndex(0) {}
};
struct GMtrl
{
	// 메터리얼 이름
	T_STR		m_strName;
	// 메터리얼 클래스 이름
	T_STR		m_strClassName;
	// 맵타입들
	vector<GTextexMap>	m_TexMaps;
	// 서브 매터리얼 저장벡터
	vector<GMtrl>		m_SubMaterial;
	GMtrl() {}
	~GMtrl()
	{
		stl_wipe_vector(m_SubMaterial);
		stl_wipe_vector(m_TexMaps);
	}
};


struct GFaceList
{
	union
	{
		struct { DWORD	_0, _1, _2; };
		DWORD v[3];
	};
	DWORD   dwMtrl;
	GFaceList() : dwMtrl(0), _0(0), _1(0), _2(0)
	{}
};

struct GVertexList
{
	DWORD					dwNumVertex;
	DWORD					dwNumFace;
	vector<D3DXVECTOR3>		pVertexList;
	vector<GFaceList>		pFaceList;
	GVertexList() : dwNumVertex(0), dwNumFace(0)
	{
	}
	~GVertexList() { stl_wipe_vector(pVertexList); stl_wipe_vector(pVertexList); }
};

struct GAnimTrack
{
	int					iTick;
	D3DXQUATERNION		qRotate;
	D3DXVECTOR3			vVector;
	GAnimTrack*			pNext;
	GAnimTrack*			pPrev;
};


typedef struct
{
	double x, y, z;
} Point3;



typedef struct
{
	/* sample time */
	double t;

	/* sample position */
	Point3 P;

	/* sample parameters for affecting tangential behavior at control point */
	double tension, continuity, bias;
}
PositionKey;

/* private structures to hide details of polynomials */
typedef struct
{
	/* P(u) = C0 + u*C1 + u^2*C2 + u^3*C3,  0 <= u <= 1 */
	Point3 C0, C1, C2, C3;

	/* sample time interval on which polynomial is valid, tmin <= t <= tmax */
	double tmin, tmax, trange;
}
CubicPolynomial;

typedef struct
{
	int numPolys;
	CubicPolynomial* poly;

	/* partial sums of arc length */
	double* length;
	double totalLength;
}
SplineInfo;