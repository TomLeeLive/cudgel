#pragma once
#include "gbsExport2016.h"

#include <vector>
using namespace std;
#ifdef __cplusplus
typedef struct _D3D_MATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float m[4][4];
	};
}D3D_MATRIX, *LPD3D_MATRIX;
#else
typedef struct _D3D_MATRIX D3D_MATRIX, *LPD3D_MATRIX;
#endif
struct GScene
{
	int iVersion;
	int iFirstFrame;
	int iLastFrame;
	int iFrameSpeed;
	int iTickPerFrame;
	int iNumMtl;
	int iNumObject;
};

struct GTex
{
	int iMapType;
	TSTR szTexName;
};
struct GMtl
{
	TSTR		szName;
	vector<GTex> texlist;
	vector<GMtl> subMtls;
};

struct PNCT_VERTEX
{
	Point3  p;
	Point3  n;
	Point4  c;
	Point2  t;
};
struct GAnimTrack
{
	int iTick;
	Quat qValue;
	Point3  vValue;
};
struct GTri
{
	PNCT_VERTEX v[3];
	int         iSubIndex;
};
struct GMesh // ÇÇºÎ, »ì
{
	int   iType; // 0 geom, 1 helper, 2 biped	
	TSTR  nodeName;
	TSTR  nodeParentName;
	D3D_MATRIX  matWorld;
	vector<GTri>  triList;
	vector<GMesh> subMesh;
	Box3  box;
	int   iRef;
	vector<GAnimTrack> posTrack;
	vector<GAnimTrack> rotTrack;
	vector<GAnimTrack> sclTrack;
};
//struct TMatrixIndex // »À
//{
//	vector<TAnimTrack> posTrack;
//	vector<TAnimTrack> rotTrack;
//	vector<TAnimTrack> sclTrack;
//};



class GbsWriter
{
public:
	TSTR			m_szFileName;
	Interface*		m_p3dsMax;
	INode*			m_pRootNode;


	vector<Mtl*>	m_MtlList;
	vector<GMtl>	m_gMtlList;

	vector<INode*>  m_ObjectList;
	vector<GMesh>   m_gObjectList;

	Interval		m_Interval;
	GScene			m_Scene;
	FILE*			m_fp;
public:
	int IsEqulVertexList(vector<PNCT_VERTEX>& UniqueVertex,
		PNCT_VERTEX& vertex);
	int ExpMesh(GMesh& pMesh, int iMtl, int iAddCount);
	BOOL EqualPoint2(Point2 t0, Point2 t1);
	BOOL EqualPoint3(Point3 t0, Point3 t1);
	BOOL EqualPoint4(Point4 t0, Point4 t1);

	BOOL EqualQuat(Quat t0, Quat t1);

	void  DumpPoint3(Point3& v, Point3& Vertex);
	void  DumpPoint2(Point2& v, Point2& Vertex);
	void  DumpQuat(Quat& v, Quat& q);
	void  DumpPoint4(Point4& v, Point4& q);
	TriObject* GetTriObjectFromNode(
		INode* pNode,
		int iTick,
		BOOL &needDel);
	void  GetMesh(INode* pNode, GMesh& gMesh);
	void  GetAnimation(INode* pNode, GMesh& tMesh);
	TSTR  FixupName(TSTR name);
	void  DumpMatrix3(Matrix3& matWorld, D3D_MATRIX& d3dWorld);
	void Init(TSTR name,
		Interface* p3dsMax);

	void PreProcess(INode* m_pRootNode);
	void AddObject(INode* pNode);
	void AddMtl(INode* pNode);



	void Release() { fclose(m_fp); };
	bool Exporter();
	bool ExpScene();
	bool ExpMaterial();
	bool ExpObject();

	void GetTexture(Mtl* pMtl, GMtl& gSubMtl);

	Point3	GetVertexNormal(Mesh* pMesh, int iFace,
		RVertex* rv);
	BOOL	TMNegParity(Matrix3 tm);
	BOOL	CheckForAnimation(INode* pNode, GMesh& tMesh,
		BOOL& IsPos,
		BOOL& IsRotate, BOOL& IsScale);
	void GetPosTrack(INode* pNode, GMesh& tMesh);
	void GetRotTrack(INode* pNode, GMesh& tMesh);
	void GetSclTrack(INode* pNode, GMesh& tMesh);
public:
	GbsWriter();
	virtual ~GbsWriter();
};

