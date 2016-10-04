#include "GMesh.h"

int GMesh::IsEqulVertexList(vector<PNCT_VERTEX>&  VertexArray, PNCT_VERTEX& Vertex)
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

int GMesh::SetUniqueBuffer(vector <GTri>& TriList, int iMtrl, int iStartTri)
{
	int iNumFaces = TriList.size();
	m_VertexArray.reserve(iNumFaces * 3);
	m_IndexArray.reserve(iNumFaces * 3);

	GTri  triSame(iMtrl);
	if (iMtrl >= 0)
	{
		iNumFaces = count_if(TriList.begin(), TriList.end(), IsTriangleSame(triSame));
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

	m_iNumFace = iNumFaces;
	return iNumFaces;
}

GMesh::GMesh()
{
	m_iNumFace = 0;
	m_iTexIndex = -1;
	m_pParent = 0;
	D3DXMatrixIdentity(&m_matWorldTrans);
	D3DXMatrixIdentity(&m_matWorldRotate);
	D3DXMatrixIdentity(&m_matWorldScale);
	D3DXMatrixIdentity(&m_matCalculation);
};


GMesh::~GMesh()
{
}
