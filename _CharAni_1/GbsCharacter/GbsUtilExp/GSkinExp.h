#pragma once
#include "GbsObject.h"

#define MAX_WEIGHT_BIPED	8
// ������������������������������������������������������������������
// ������ 1�� �� �Ҵ��ʿ�.
// ������������������������������������������������������������������
struct TBipedVertex
{
	int					m_dwNumWeight;
	vector<BYTE>		m_BipIDList;
	vector<float>		m_fWeightList;	
};
// ������������������������������������������������������������������
//  ���̴� ����( ����ġ 4�� ���� ��� )
// ������������������������������������������������������������������
struct P3NCT3VERTEX
{	
	Point3		p;	
	Point3		n;
	Point4		c;
	Point2		t;
	float		w[4];// W0, fW1, fW2, fNumWeight;
	float		i[4];	// I0, I1, I2, I3
};
// ������������������������������������������������������������������
//  ���̴� ����( ����ġ 8�� ���� ��� )
// ������������������������������������������������������������������
struct PNCT5_VERTEX
{	
	Point3		p;	
	Point3		n;
	Point4		c;
	Point2		t;
	float		w1[4];// W0, fW1, fW2, fW3;
	float		i1[4];	// I0, I1, I2, I3
	float		w2[4];// W4, fW5, fW6, fNumWeight;	
	float		i2[4];	// I4, I5, I6, I7
};
struct TSkinTri : public TTri
{
	PNCT5_VERTEX	m_vVertex[3];
};
struct TSkinMesh : public TMesh
{
	vector<TSkinTri>		m_SkinTriList;	
	vector<TSkinMesh*>		m_pSkinSubMesh;
	// �ؽ��� ����( �ؽ��� �� ���� )
	int						m_iNumTex;
	int						m_iTexType[13];
	TCHAR					m_szTexName[13][30];	
	TSkinMesh() {};
	virtual ~TSkinMesh() {};
};
// ������������������������������������������������������������������
// 
// ������������������������������������������������������������������
class TSkinExport : public TSingleton< TSkinExport >
{
private:
	friend class TSingleton<TSkinExport>;
public:
	TScene			m_Scene;
	// ��������ġ ����Ʈ
	typedef vector<TBipedVertex>	TBIPED_VERTEX_VECTOR;
	TBIPED_VERTEX_VECTOR			m_BipedList;
	// �޽� ���̽� ����Ʈ
	typedef vector<TSkinMesh*>		TSKINMESH_VECTOR;
	TSKINMESH_VECTOR				m_SkinMeshList;
public:
	bool		Init();
	bool		skmExport();
	bool		Release();
	Modifier*	FindModifier( INode *nodePtr, Class_ID classID );
	void		SetBipedInfo( INode* node );
	void		ExportPhysiqueData(INode* node, Modifier	*phyMod);
	void		ExportSkinData(INode* node, Modifier	*skinMod);	
	bool		ExportMesh(FILE* fp, TSkinMesh* pMesh );
	void		SetVertexBiped(	INode* node, Face*	face, 
								int v0, int v1, int v2, 
								TSkinTri* pTri );
	bool		GetNodeInfo( INode* node, TimeValue t  );
	// ������͸��� ���ø� ����Ͽ� ���̽� ����Ʈ ����
	void		GenerateGroup( INode* node, Mesh *mesh, TSkinMesh* pSkinMesh );
	void		LoadMaterial( TSkinMesh*	pSkinMesh, Mtl* mtl);
	void		SetTriangle( INode* node, Mesh *mesh, TSkinMesh* pSkinMesh, TSkinMesh*	pSumMesh = NULL, int iMtrl = 0 );	
	int			GetMapID(Class_ID cid, int subNo);
	int			IsEqulVertexList( vector<PNCT5_VERTEX>&  VertexArray, PNCT5_VERTEX& Vertex );
public:
	TSkinExport(void);
	virtual ~TSkinExport(void);
};
#define I_SkinExp TSkinExport::GetInstance()
