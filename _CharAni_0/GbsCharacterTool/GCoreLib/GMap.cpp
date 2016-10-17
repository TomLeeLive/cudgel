#include "GMap.h"
DXGI_FORMAT GMap::MAKE_TYPELESS(DXGI_FORMAT format)
{
	/*  if( !DXUTIsInGammaCorrectMode() )
	return format;*/

	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;

	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_TYPELESS;
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_TYPELESS;
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_TYPELESS;
	};

	return format;
}
//--------------------------------------------------------------------------------------
// Helper functions to create SRGB formats from typeless formats and vice versa
//--------------------------------------------------------------------------------------
DXGI_FORMAT GMap::MAKE_SRGB(DXGI_FORMAT format)
{
	/*  if( !DXUTIsInGammaCorrectMode() )
	return format;*/

	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	};
	return format;
}
D3DXVECTOR3 GMap::ComputeFaceNormal(DWORD dwIndex0, DWORD dwIndex1, DWORD dwIndex2)
{
	D3DXVECTOR3 vNormal;
	D3DXVECTOR3 v0 = m_pvHeightMap[dwIndex1].p - m_pvHeightMap[dwIndex0].p;
	D3DXVECTOR3 v1 = m_pvHeightMap[dwIndex2].p - m_pvHeightMap[dwIndex0].p;

	D3DXVec3Cross(&vNormal, &v0, &v1);
	D3DXVec3Normalize(&vNormal, &vNormal);
	return vNormal;
}
HRESULT GMap::SetInputLayout()
{
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_Object.g_pInputlayout.Attach(DX::CreateInputlayout(m_pd3dDevice, m_Object.g_pVSBlob.Get()->GetBufferSize(),
		m_Object.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return hr;
}
HRESULT GMap::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	m_Object.m_iVertexSize = sizeof(PNCT_VERTEX);
	m_Object.m_iNumVertex = m_iNumVertices;
	m_Object.g_pVertexBuffer.Attach(DX::CreateVertexBuffer(m_pd3dDevice, m_pvHeightMap, m_Object.m_iNumVertex, m_Object.m_iVertexSize));
	return hr;
}
HRESULT GMap::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	m_Object.m_iNumIndex = m_iNumFace * 3;//(m_iNumRows - 1)*(m_iNumCols - 1) * 2 * 3
	m_Object.g_pIndexBuffer.Attach(DX::CreateIndexBuffer(m_pd3dDevice, m_pdwIndexArray, m_Object.m_iNumIndex, sizeof(DWORD)));
	return hr;
}
bool GMap::CreateVertexList()
{
	if (m_pvHeightMap == nullptr)
	{
		/////////////////////////////////////////////////////////////////
		// 정점 리스트
		/////////////////////////////////////////////////////////////////
		SAFE_NEW_ARRAY(m_pvHeightMap, PNCT_VERTEX, m_iNumVertices);	// 높이맵배열 생성	
	}

	int iHalfCols = m_iNumCols / 2;
	int iHalfRows = m_iNumRows / 2;
	float ftxOffsetU = 1.0f / (m_iNumCols - 1);
	float ftxOffsetV = 1.0f / (m_iNumRows - 1);

	for (int iRow = 0; iRow < m_iNumRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumCols; iCol++)
		{
			int  iVertexIndex = iRow * m_iNumCols + iCol;
			m_pvHeightMap[iVertexIndex].p.x = (iCol - iHalfCols)		* m_fSellDistance;
			m_pvHeightMap[iVertexIndex].p.z = -((iRow - iHalfRows)	* m_fSellDistance);
			m_pvHeightMap[iVertexIndex].p.y = GetHeightOfVertex(iVertexIndex);
			m_pvHeightMap[iVertexIndex].n = GetNormalOfVertex(iVertexIndex);
			m_pvHeightMap[iVertexIndex].c = GetColorOfVertex(iVertexIndex);
			m_pvHeightMap[iVertexIndex].t.x = ((iCol%2)==0)?0.0f : 1.0f;//ftxOffsetU *iCol;
			m_pvHeightMap[iVertexIndex].t.y = ((iRow % 2) == 0) ? 0.0f : 1.0f;// ftxOffsetV *iRow;
		}
	}
	return true;
}
bool GMap::CreateIndexList()
{
	if (m_pdwIndexArray == nullptr)
	{
		/////////////////////////////////////////////////////////////////
		// 인덱스 리스트 
		/////////////////////////////////////////////////////////////////
		SAFE_NEW_ARRAY(m_pdwIndexArray, DWORD, m_iNumFace * 3);
	}

	int iCurIndex = 0;
	for (int iRow = 0; iRow < m_iNumSellRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumSellCols; iCol++)
		{
			//0	1    4   
			//2	   3 5
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			m_pdwIndexArray[iCurIndex + 0] = iRow * m_iNumCols + iCol;
			m_pdwIndexArray[iCurIndex + 1] = iRow * m_iNumCols + iNextCol;
			m_pdwIndexArray[iCurIndex + 2] = iNextRow * m_iNumCols + iCol;
			m_pdwIndexArray[iCurIndex + 3] = m_pdwIndexArray[iCurIndex + 2];
			m_pdwIndexArray[iCurIndex + 4] = m_pdwIndexArray[iCurIndex + 1];
			m_pdwIndexArray[iCurIndex + 5] = iNextRow * m_iNumCols + iNextCol;

			iCurIndex += 6;
		}
	}

	return true;
}
bool GMap::CreateMap(GMapDesc& MapDesc)
{
	m_iNumRows = MapDesc.iNumCols;
	m_iNumCols = MapDesc.iNumRows;
	m_iNumSellRows = m_iNumRows - 1;
	m_iNumSellCols = m_iNumCols - 1;
	m_iNumVertices = m_iNumRows*m_iNumCols;
	m_iNumFace = m_iNumSellRows*m_iNumSellCols * 2;
	m_fSellDistance = MapDesc.fSellDistance;
	if (!CreateVertexList()) return false;
	if (!CreateIndexList()) return false;

	///////////////////////////////////////////////////////////////////
	//// 페이스 노말 계산 및  이웃 페이스 인덱스 저장하여 정점 노말 계산
	///////////////////////////////////////////////////////////////////
	InitFaceNormals();
	GenNormalLookupTable();
	CalcPerVertexNormalsFastLookup();

	/////////////////////////////////////////////////////////////////
	// 페이스 노말 계산 및  이웃 페이스 인덱스 저장하여 정점 노말 계산
	/////////////////////////////////////////////////////////////////	
	CalcVertexColor(m_vLightDir);
	return true;
}

float GMap::GetHeightOfVertex(UINT Index)
{
	return 0.0f;
};
D3DXVECTOR3 GMap::GetNormalOfVertex(UINT Index)
{
	return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
};
D3DXVECTOR4 GMap::GetColorOfVertex(UINT Index)
{
	return D3DXVECTOR4(1, 1, 1, 1.0f);
};
void GMap::CalcVertexColor(D3DXVECTOR3 vLightDir)
{
	/////////////////////////////////////////////////////////////////
	// 페이스 노말 계산 및  이웃 페이스 인덱스 저장하여 정점 노말 계산
	/////////////////////////////////////////////////////////////////	
	for (int iRow = 0; iRow < m_iNumRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumCols; iCol++)
		{
			int  iVertexIndex = iRow * m_iNumCols + iCol;
			D3DXVec3Normalize(&m_pvHeightMap[iVertexIndex].n, &m_pvHeightMap[iVertexIndex].n);
			float fDot = max(0.1f, D3DXVec3Dot(&-vLightDir, &m_pvHeightMap[iVertexIndex].n));
			m_pvHeightMap[iVertexIndex].c *= fDot;
			m_pvHeightMap[iVertexIndex].c.w = 1.0f;
		}
	}
}
bool GMap::ReLoadVBuffer()
{
	CalcPerVertexNormalsFastLookup();
	m_pContext->UpdateSubresource(m_Object.g_pVertexBuffer.Get(), 0, NULL, m_pvHeightMap, 0, 0);
	return true;
}
bool GMap::ReLoadIBuffer()
{
	m_pContext->UpdateSubresource(m_Object.g_pIndexBuffer.Get(), 0, NULL, m_pdwIndexArray, 0, 0);
	return true;
}
bool GMap::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext*  pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	return true;
}
bool GMap::Load(GMapDesc& MapDesc)
{
	if (!CreateMap(MapDesc))
	{
		return false;
	}

	if (!GShape::Create(m_pd3dDevice, MapDesc.strShaderFile.c_str(), MapDesc.strTextureFile.c_str()))
	{
		return false;
	}

	return true;
}

bool GMap::Render(ID3D11DeviceContext*  pContext)
{
	PreRender(pContext);
	PostRender(pContext);
	return true;
}
bool GMap::Frame()
{
	return true;
}
bool GMap::Release()
{
	if (m_pNormalLookupTable)
	{
		free(m_pNormalLookupTable);
		m_pNormalLookupTable = NULL;
	}
	if (m_pFaceNormals != NULL)
	{
		delete m_pFaceNormals;
		m_pFaceNormals = NULL;
	}
	SAFE_DELETE_ARRAY(m_pvHeightMap);
	SAFE_DELETE_ARRAY(m_pdwIndexArray);
	return true;
}
void GMap::InitFaceNormals()
{
	m_pFaceNormals = new D3DXVECTOR3[(m_iNumRows - 1)*(m_iNumCols - 1) * 2];

	for (int i = 0; i<(m_iNumRows - 1)*(m_iNumCols - 1) * 2; i++)
	{
		m_pFaceNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}
//--------------------------------------------------------------------------------------
// Create face normals for the mesh
//--------------------------------------------------------------------------------------
void GMap::CalcFaceNormals()
{
	// Loop over how many faces there are
	int j = 0;
	for (int i = 0; i< m_iNumFace * 3; i += 3)
	{
		DWORD i0 = m_pdwIndexArray[i];
		DWORD i1 = m_pdwIndexArray[i + 1];
		DWORD i2 = m_pdwIndexArray[i + 2];
		m_pFaceNormals[j] = ComputeFaceNormal(i0, i1, i2);
		j++;
	}
}
//--------------------------------------------------------------------------------------
// Create a fast normal lookup table
//--------------------------------------------------------------------------------------
void GMap::GenNormalLookupTable()
{
	// We're going to create a table that lists, for each vertex, the
	// triangles which that vertex is a part of.

	if (m_pNormalLookupTable != NULL)
	{
		free(m_pNormalLookupTable);
		m_pNormalLookupTable = NULL;
	}

	// Each vertex may be a part of up to 6 triangles in the grid, so
	// create a buffer to hold a pointer to the normal of each neighbor.
	int buffersize = m_iNumRows*m_iNumCols * 6;

	m_pNormalLookupTable = (int *)malloc(sizeof(void *) * buffersize);
	for (int i = 0; i<buffersize; i++)
		m_pNormalLookupTable[i] = -1;

	// Now that the table is initialized, populate it with the triangle data.

	// For each triangle
	//   For each vertex in that triangle
	//     Append the triangle number to lookuptable[vertex]
	for (int i = 0; i < m_iNumFace; i++)
	{
		for (int j = 0; j<3; j++)
		{
			// Find the next empty slot in the vertex's lookup table "slot"
			for (int k = 0; k<6; k++)
			{
				int vertex = m_pdwIndexArray[i * 3 + j];
				if (m_pNormalLookupTable[vertex * 6 + k] == -1)
				{
					m_pNormalLookupTable[vertex * 6 + k] = i;
					break;
				}
			}
		}  // For each vertex that is part of the current triangle
	} // For each triangle
}

//--------------------------------------------------------------------------------------
// Compute vertex normals from the fast normal lookup table
//--------------------------------------------------------------------------------------
void GMap::CalcPerVertexNormalsFastLookup()
{
	// First, calculate the face normals for each triangle.
	CalcFaceNormals();

	// For each vertex, sum the normals indexed by the normal lookup table.
	int j = 0;
	for (int i = 0; i < m_iNumVertices; i++)
	{
		D3DXVECTOR3 avgNormal;
		avgNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// Find all the triangles that this vertex is a part of.
		for (j = 0; j<6; j++)
		{
			int triIndex;
			triIndex = m_pNormalLookupTable[i * 6 + j];

			// If the triangle index is valid, get the normal and average it in.
			if (triIndex != -1)
			{
				avgNormal += m_pFaceNormals[triIndex];
			}
			else
				break;
		}

		// Complete the averaging step by dividing & normalizing.
		_ASSERT(j > 0);
		avgNormal.x /= (float)j;//.DivConst( (float)(j) );
		avgNormal.y /= (float)j;
		avgNormal.z /= (float)j;
		D3DXVec3Normalize(&avgNormal, &avgNormal);

		// Set the vertex normal to this new normal.
		m_pvHeightMap[i].n.x = avgNormal.x;
		m_pvHeightMap[i].n.y = avgNormal.y;
		m_pvHeightMap[i].n.z = avgNormal.z;

	}  // For each vertex
}

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT GMap::CreateResource()
{
	return S_OK;
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT GMap::DeleteResource()
{
	return S_OK;
}
GMap::GMap(void)
{
	m_pvHeightMap = nullptr;
	m_pdwIndexArray = nullptr;
	m_iNumFace = 0;
	m_iTexIndex = 0;
	m_iNumCols = 0;
	m_iNumRows = 0;
	m_pd3dDevice = nullptr;
	m_pNormalLookupTable = nullptr;
	m_pFaceNormals = nullptr;
	m_vLightDir = D3DXVECTOR3(0, -1, 0);
}

GMap::~GMap(void)
{
}
