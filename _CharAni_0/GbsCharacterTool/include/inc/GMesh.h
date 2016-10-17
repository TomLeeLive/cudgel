#pragma once
#include "GObjStd.h"
using namespace DX;

struct GTri
{
	PNCT_VERTEX	 vVertex[3];
	D3DXVECTOR3  vNormal;
	int			 iSubIndex;
	GTri(int Index) : iSubIndex(Index) {}
	GTri() : iSubIndex(-1) {}
};
struct DescendingTriSort
{
	bool operator()(GTri& rpStart, GTri& rpEnd)
	{
		return rpStart.iSubIndex < rpEnd.iSubIndex;
	}
};
struct IsTriangleSame
{
	GTri			m_Tri;
	IsTriangleSame(GTri data) : m_Tri(data) {}
	bool operator()(GTri& value)
	{
		return value.iSubIndex == m_Tri.iSubIndex;
	}
};


class GMesh : public GShape
{
public:
	T_STR					m_strNodeName;
	T_STR					m_strParentName;
	OBJECTCLASSTYPE			m_ClassType;
	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matInverse;
	D3DXMATRIX				m_matWorldTrans;
	D3DXMATRIX				m_matCalculation;
	D3DXMATRIX				m_matWorldRotate;
	D3DXMATRIX				m_matWorldScale;
	bool					m_bUsed;
	int						m_iNumFace;
	int						m_iTexIndex;
	G_BOX					m_BBox;
	vector<PNCT_VERTEX>		m_VertexArray;
	vector<GTri>		m_TriList;
	GMesh*				m_pParent;
	vector<DWORD>		m_IndexArray;
	vector<shared_ptr<GMesh>>		m_pSubMesh;
	vector<GMesh*>		m_pChildMesh;
	vector<shared_ptr<GAnimTrack>>		m_pPosTrack;
	vector<shared_ptr<GAnimTrack>>		m_pRotTrack;
	vector<shared_ptr<GAnimTrack>>		m_pSclTrack;
	vector<shared_ptr<GAnimTrack>>		m_pVisTrack;
	float					m_fVisibility;
public:
	int IsEqulVertexList(vector<PNCT_VERTEX>&  VertexArray, PNCT_VERTEX& Vertex);
	int SetUniqueBuffer(vector <GTri>& TriList, int iMtrl, int iStartTri);
public:
	GMesh(void);
	virtual ~GMesh(void);
};

