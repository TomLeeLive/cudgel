#pragma once

#include "GModel.h"


using namespace DX;

template< class T > struct GTri
{
	T						vVertex[3];
	D3DXVECTOR3  vNormal;
	int						iSubIndex;
	GTri(int Index) : iSubIndex(Index) {}
	GTri() : iSubIndex(-1) {}
};
template< class T > struct DescendingTriSort
{
	bool operator()(GTri<T>& rpStart, GTri<T>& rpEnd)
	{
		return rpStart.iSubIndex < rpEnd.iSubIndex;
	}
};
template< class T > struct IsTriangleSame
{
	GTri<T>			m_Tri;
	IsTriangleSame(GTri<T> data) : m_Tri(data) {}
	bool operator()(GTri<T>& value)
	{
		return value.iSubIndex == m_Tri.iSubIndex;
	}
};

template <class T > class GData
{
public:
	int					m_iNumFace;
	vector<T>			m_VertexArray;
	vector<DWORD>		m_IndexArray;
	vector<GTri<T>>		m_TriList;
	vector<shared_ptr<GData>>		m_pSubMesh;
	vector<GData*>		m_pChildMesh;
	T*					m_pDrawVertex;
	D3DXMATRIX				m_matInverse;
public:
	int IsEqulVertexList(vector<T>&  VertexArray, T& Vertex);
	int SetUniqueBuffer(vector <GTri<T>>& TriList, int iMtrl, int iStartTri);
	GData(void)
	{
		m_pDrawVertex = NULL;
	}
};

class GMesh : public GModel
{
public:
	int						m_iIndex;
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
	int						m_iDiffuseTex;
	G_BOX					m_BBox;
	GMesh*				m_pParent;
	
	vector<shared_ptr<GMesh>>		m_pSubMesh;
	vector<GMesh*>		m_pChildMesh;
	vector<shared_ptr<TAnimTrack>>		m_pPosTrack;
	vector<shared_ptr<TAnimTrack>>		m_pRotTrack;
	vector<shared_ptr<TAnimTrack>>		m_pSclTrack;
	vector<shared_ptr<TAnimTrack>>		m_pVisTrack;
	float					m_fVisibility;

	//TBS
	int   m_iNumTrack[4];
	int   m_iMtrlRef;

	// skm
	// 텍스쳐 정보( 텍스쳐 맵 단위 )	
	int						m_iNumMtrl;
	int						m_iNumVertexs;

	int						m_iNumTex;
	int						m_iTexType[13];
	TCHAR					m_szTexName[13][30];

	ComPtr<ID3D11CommandList>      m_pd3dCommandList;
public:
	GMesh(void)
	{
		m_iNumFace = 0;
		m_iDiffuseTex = -1;
		m_iIndex = -1;
		m_pParent = 0;
		m_iNumVertexs = 0;
		m_iNumMtrl = 0;
		D3DXMatrixIdentity(&m_matWorldTrans);
		D3DXMatrixIdentity(&m_matWorldRotate);
		D3DXMatrixIdentity(&m_matWorldScale);
		D3DXMatrixIdentity(&m_matCalculation);
	};;
	virtual ~GMesh(void) {};
};

template <class T >
int GData<T>::IsEqulVertexList(vector<T>&  VertexArray, T& Vertex)
{
	for (int iVertex = 0; iVertex < VertexArray.size(); iVertex++)
	{
		if (VertexArray[iVertex] == Vertex)
		{
			return iVertex;
		}
	}
	return -1;
}

template <class T >
int GData<T>::SetUniqueBuffer(vector <GTri<T>>& TriList, int iMtrl, int iStartTri)
{
	int iNumFaces = TriList.size();
	m_VertexArray.reserve(iNumFaces * 3);
	m_IndexArray.reserve(iNumFaces * 3);

	GTri<T>  triSame(iMtrl);
	if (iMtrl >= 0)
	{
		iNumFaces = count_if(TriList.begin(), TriList.end(), IsTriangleSame<T>(triSame));
	}

	for (int iFace = 0; iFace < iNumFaces; iFace++)
	{
		for (int iCnt = 0; iCnt < 3; iCnt++)
		{
			int iPosReturn = IsEqulVertexList(m_VertexArray, TriList[iStartTri + iFace].vVertex[iCnt]);
			if (iPosReturn < 0)
			{
				m_VertexArray.push_back(TriList[iStartTri + iFace].vVertex[iCnt]);
				iPosReturn = m_VertexArray.size() - 1;
			}
			m_IndexArray.push_back(iPosReturn);
		}
	}

	return iNumFaces;
}
