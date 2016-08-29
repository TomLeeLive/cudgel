#pragma once
#include <GBasisLib_0.h>
#include <GShape.h>
#include "GParser.h"
/*
static TCHAR* AseSections[] = { _T("SCENE "),_T("MATERIAL_LIST"),_T("GEOMOBJECT"),_T("HELPEROBJECT"), };
enum AseSectionType { SCENE = 0, MATERIALLIST, GEOMOBJECT, HELPEROBJECT, };

static TCHAR* AseMaterialTokens[] = { _T("NUMSUBMTLS"),	_T("MAP_"),_T("}"), };
enum AseMaterialType { NUM_SUBMTRLS = 0, MAP, };

static TCHAR* AseTextureTokens[] = { _T("BITMAP "),		_T("}"), };
enum AseTextureType { BITMAP_FILE = 0, };

static TCHAR* AseGeomObjectTokens[] = {
_T("NODE_PARENT"),
_T("NODE_TM"),
_T("MESH {"),
_T("TM_ANIMATION"),
_T("NODE_VISIBILITY_TRACK"),
_T("MATERIAL_REF"),

_T("GEOMOBJECT"),	// Return
_T("HELPEROBJECT"),
};
enum AseGeomObjectType {
NODE_PARENT = 0, MODE_TM,
MESH, TM_ANIMATION, TM_VISIBILITY, MATERIAL_REF,
};
*/

ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11Buffer*           g_pIndexBuffer = NULL;
ID3D11Buffer*           g_pConstantBuffer = NULL;
D3DXMATRIX                g_World;
D3DXMATRIX                g_View;
D3DXMATRIX                g_Projection;

struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR4 Color;
};

struct ConstantBuffer
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};



TCHAR* g_pAseItems[] = { L"*SCENE",L"*MATERIAL_LIST",L"*GEOMOBJECT"/*,L"HELPEROBJECT"*/ };
enum AseItemsType { SCENE = 0, MATERIALLIST, GEOMOBJECT, /*HELPEROBJECT, */};

TCHAR* g_pAseSceneTokens[] = { L"*SCENE_FILENAME", L"*SCENE_FIRSTFRAME", L"*SCENE_LASTFRAME",L"*SCENE_FRAMESPEED",L"*SCENE_TICKSPERFRAME" };
TCHAR* g_pAseMaterialTokens[] = {L"*MATERIAL_NAME",L"*MATERIAL_AMBIENT", L"*MATERIAL_DIFFUSE", L"*MATERIAL_SPECULAR",L"*BITMAP" };

TCHAR* g_pAseObjTokens[] = { L"*NODE_NAME",L"*NODE_TM",L"*MESH" };
enum AseObjTokensType {NODE_NAME = 0, NODE_TM, MESH};

TCHAR* g_pAseNodeTmTokens[] = { L"*TM_ROW0",L"*TM_ROW1", L"*TM_ROW2", L"*TM_ROW3" };

struct GAseScene {
	// Scene정보들..
	TCHAR  m_szName[MAX_PATH];				//*SCENE_FILENAME "Box.max"
	int	   m_iFrame;						//*SCENE_FIRSTFRAME 0
	int	   m_iLastFrame;					//*SCENE_LASTFRAME 100
	int	   m_iFrameSpeed;					//*SCENE_FRAMESPEED 30
	int	   m_iTicksPerFrame;				//*SCENE_TICKSPERFRAME 160
};

struct GAseMaterial {
	TCHAR		m_szName[MAX_PATH];			//*MATERIAL_NAME "01 - Default"
	D3DXVECTOR3 m_vecAmbient;				//*MATERIAL_AMBIENT 0.5882	0.5882	0.5882
	D3DXVECTOR3 m_vecDiffuse;				//*MATERIAL_DIFFUSE 0.5882	0.5882	0.5882
	D3DXVECTOR3 m_vecSpecular;				//*MATERIAL_SPECULAR 0.9000	0.9000	0.9000
	TCHAR		m_szMapDiffuse[MAX_PATH];	//*BITMAP "C:\TBasis200\Data\object\textures\flagstone.bmp"
	GAseMaterial() {};
	~GAseMaterial() {};
};
struct GAseObj {
	TCHAR		m_szName[MAX_PATH];		//*NODE_NAME "Box01"
	D3DXMATRIX m_matWorld;				//월드행렬
	int					m_iPosCount;	//Vertex 카운트
	int					m_iFaceCount;	//Face 카운트
	vector<D3DXVECTOR3> m_vPosList;		//Vec3
	vector<D3DXVECTOR3> m_vNorList;		//Vec3
	vector<D3DXVECTOR3> m_vColList;		//Vec3
	vector<D3DXVECTOR3> m_vTexList;		//Vec3

	vector<PNCT_VERTEX> m_vVertex;		//VB
	vector<int>		m_vIndex;		//IB
};

struct GAseModel {
	GAseScene					m_stScene;
	vector<shared_ptr<GAseMaterial>>		m_vMaterial;
	vector<shared_ptr<GAseObj>>			m_vObj;
};

class Sample : public GBASISLib_0
{
public:
	GAseModel m_stModel;
	GParser	  m_Parser;

	int		GetObjDataFromFile() {
		
		//GAseObj* aseobj = new GAseObj;
		auto aseobj = make_shared<GAseObj>();

		int iSize = sizeof(g_pAseObjTokens) / sizeof(g_pAseObjTokens[0]);

		while (!feof(m_Parser.m_pStream))
		{
			_fgetts(m_Parser.m_pBuffer, 256, m_Parser.m_pStream);
			_stscanf(m_Parser.m_pBuffer, _T("%s"), m_Parser.m_pString);


			for (int i = 0; i < iSize; i++) {


				if (!_tcsicmp(m_Parser.m_pString, g_pAseObjTokens[i]))
				{
					switch (i)
					{

					case NODE_NAME:
					{

						m_Parser.GetData(&(aseobj->m_szName), STRING_DATA);
					}
					break;
					case NODE_TM:
					{
						D3DXVECTOR3 vecROW0;
						D3DXVECTOR3 vecROW1;
						D3DXVECTOR3 vecROW2;
						D3DXVECTOR3 vecROW3;
						m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[0], &vecROW0, VERTEX_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[1], &vecROW1, VERTEX_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[2], &vecROW2, VERTEX_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[3], &vecROW3, VERTEX_DATA);
						
						D3DXMatrixIdentity(&aseobj->m_matWorld);

						aseobj->m_matWorld._11 = vecROW0.x; aseobj->m_matWorld._12 = vecROW0.y; aseobj->m_matWorld._13 = vecROW0.z;
						aseobj->m_matWorld._31 = vecROW1.x; aseobj->m_matWorld._32 = vecROW1.y; aseobj->m_matWorld._33 = vecROW1.z;
						aseobj->m_matWorld._21 = vecROW2.x; aseobj->m_matWorld._22 = vecROW2.y; aseobj->m_matWorld._23 = vecROW2.z;
						aseobj->m_matWorld._41 = vecROW3.x; aseobj->m_matWorld._42 = vecROW3.y; aseobj->m_matWorld._43 = vecROW3.z;

					}
					break;
					case MESH:
					{
						ST_MESH_FACE stMeshFace;
						//int			iNumVertex,iNumFaces;
						D3DXVECTOR3 pPos;
						m_Parser.GetDataFromFileLoop(L"*MESH_NUMVERTEX", &(aseobj->m_iPosCount), INT_DATA);
						m_Parser.GetDataFromFileLoop(L"*MESH_NUMFACES", &(aseobj->m_iFaceCount), INT_DATA);

						for (int i = 0; i < aseobj->m_iPosCount; i++) {
							//pPos = new D3DXVECTOR3;
							m_Parser.GetDataFromFileLoop(L"*MESH_VERTEX", &pPos, MESH_VERTEX_DATA);
							aseobj->m_vPosList.push_back(pPos);
						}

						for (int i = 0; i < aseobj->m_iFaceCount; i++) {
							m_Parser.GetDataFromFileLoop(L"*MESH_FACE", &stMeshFace, MESH_FACE_DATA);
							aseobj->m_vIndex.push_back(stMeshFace.index1);
							aseobj->m_vIndex.push_back(stMeshFace.index2);
							aseobj->m_vIndex.push_back(stMeshFace.index3);
						}
						
					}
					break;

					}
				}
			}
			//fgetc(m_Parser.m_pStream);
		}

		m_stModel.m_vObj.push_back(aseobj);
		return -1;

	}
	int		GetDataFromFile() {
		
		int iSize = sizeof(g_pAseItems) / sizeof(g_pAseItems[0]);

		while (!feof(m_Parser.m_pStream))
		{
			_fgetts(m_Parser.m_pBuffer, 256, m_Parser.m_pStream);
			_stscanf(m_Parser.m_pBuffer, _T("%s"), m_Parser.m_pString);

			
			for (int i = 0; i < iSize; i++){

				if (!_tcsicmp(m_Parser.m_pString, g_pAseItems[i]))
				{
					switch (i)
					{
					case SCENE: 
					{
						m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[0], &m_stModel.m_stScene.m_szName, STRING_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[1], &m_stModel.m_stScene.m_iFrame, INT_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[2], &m_stModel.m_stScene.m_iLastFrame, INT_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[3], &m_stModel.m_stScene.m_iFrameSpeed, INT_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[4], &m_stModel.m_stScene.m_iTicksPerFrame, INT_DATA);
					}
						break;
					case MATERIALLIST: 
					{
						//GAseMaterial* material = new GAseMaterial;
						auto material = make_shared<GAseMaterial>();
						m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[0], &(material->m_szName), STRING_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[1], &(material->m_vecAmbient), VERTEX_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[2], &(material->m_vecDiffuse), VERTEX_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[3], &(material->m_vecSpecular), VERTEX_DATA);
						m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[4], &(material->m_szMapDiffuse), STRING_DATA);
						m_stModel.m_vMaterial.push_back(material);
					}
						break;
					case GEOMOBJECT: 
					{
						GetObjDataFromFile();
					}
						break;
					}
				}
			}
			//fgetc(m_Parser.m_pStream);
		}

		return -1;
	}
	bool		Init() { 
		m_Parser.OpenStream(L"data/BOX.ASE");


		
		GetDataFromFile();

		//m_Parser.GetDataFromFileLoop(g_pAseItems[0], &pData, NULL_DATA);

		//GetDataFromFileLoop(g_pAseItems[0],);
		m_Parser.CloseStream();



		HRESULT hr = S_OK;

		// Compile the vertex shader
		ID3DBlob* pVSBlob = NULL;
		hr = CompileShaderFromFile(L"Tutorial04.fx", "VS", "vs_4_0", &pVSBlob);
		if (FAILED(hr))
		{
			MessageBox(NULL,
				L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
			return hr;
		}

		// Create the vertex shader
		hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
		if (FAILED(hr))
		{
			pVSBlob->Release();
			return hr;
		}

		// Define the input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		// Create the input layout
		hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(), &g_pVertexLayout);
		pVSBlob->Release();
		if (FAILED(hr))
			return hr;

		// Set the input layout
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

		// Compile the pixel shader
		ID3DBlob* pPSBlob = NULL;
		hr = CompileShaderFromFile(L"Tutorial04.fx", "PS", "ps_4_0", &pPSBlob);
		if (FAILED(hr))
		{
			MessageBox(NULL,
				L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
			return hr;
		}

		// Create the pixel shader
		hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
		pPSBlob->Release();
		if (FAILED(hr))
			return hr;

		// Create vertex buffer
		//m_stModel.m_vObj[0]->m_vVertex[0].p
		//m_stModel.m_vObj[0]->m_vVertex.

		SimpleVertex* vertices;
		vertices = (SimpleVertex *)malloc(sizeof(SimpleVertex) * m_stModel.m_vObj[0]->m_iPosCount);


		for (int i = 0; i < m_stModel.m_vObj[0]->m_iPosCount; i++) {
			vertices[i] = { m_stModel.m_vObj[0]->m_vPosList[i], D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f) };
		}
		/*
		SimpleVertex vertices[] =
		{
			//{ m_stModel.m_vObj[0]->m_vPosList[0], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) },
			//{ m_stModel.m_vObj[0]->m_vPosList[1], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) },
			//{ m_stModel.m_vObj[0]->m_vPosList[2], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) },
			//{ m_stModel.m_vObj[0]->m_vPosList[3], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) },
			//{ m_stModel.m_vObj[0]->m_vPosList[4], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) },
			//{ m_stModel.m_vObj[0]->m_vPosList[5], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) },
			//{ m_stModel.m_vObj[0]->m_vPosList[6], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) },
			//{ m_stModel.m_vObj[0]->m_vPosList[7], D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.0f),D3DXVECTOR2(0.0f, 0.0f) }

			{ m_stModel.m_vObj[0]->m_vPosList[0], D3DXVECTOR4(1.0f, 0.0f, 1.0f,1.0f) },
			{ m_stModel.m_vObj[0]->m_vPosList[1], D3DXVECTOR4(0.0f, 1.0f, 1.0f,1.0f) },
			{ m_stModel.m_vObj[0]->m_vPosList[2], D3DXVECTOR4(1.0f, 1.0f, 0.0f,1.0f) },
			{ m_stModel.m_vObj[0]->m_vPosList[3], D3DXVECTOR4(1.0f, 0.0f, 1.0f,1.0f) },
			{ m_stModel.m_vObj[0]->m_vPosList[4], D3DXVECTOR4(0.0f, 1.0f, 1.0f,1.0f) },
			{ m_stModel.m_vObj[0]->m_vPosList[5], D3DXVECTOR4(1.0f, 1.0f, 0.0f,1.0f) },
			{ m_stModel.m_vObj[0]->m_vPosList[6], D3DXVECTOR4(1.0f, 0.0f, 1.0f,1.0f) },
			{ m_stModel.m_vObj[0]->m_vPosList[7], D3DXVECTOR4(0.0f, 1.0f, 1.0f,1.0f) }
		};
		*/

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * m_stModel.m_vObj[0]->m_iPosCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;
		hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
		if (FAILED(hr))
			return hr;

		// Set vertex buffer
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

		// Create index buffer


		WORD* indices;
		indices = (WORD *)malloc(sizeof(WORD) * m_stModel.m_vObj[0]->m_iFaceCount * 3);


		for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {

			if(i == 0 || i% 3 ==0){
				indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i]) };
			}
			else if (i ==1 || i%3 ==1) {
				indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i+1]) };
			}
			else if( i ==2 || i%3 ==2 ) {
				indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i-1]) };
			}

		}
		/*
		WORD indices[] =
		{
			{m_stModel.m_vObj[0]->m_vIndex[0]},
			{ m_stModel.m_vObj[0]->m_vIndex[1] },
			{ m_stModel.m_vObj[0]->m_vIndex[2] },
			{ m_stModel.m_vObj[0]->m_vIndex[3] },
			{ m_stModel.m_vObj[0]->m_vIndex[4] },
			{ m_stModel.m_vObj[0]->m_vIndex[5] },
			{ m_stModel.m_vObj[0]->m_vIndex[6] },
			{ m_stModel.m_vObj[0]->m_vIndex[7] },
			{ m_stModel.m_vObj[0]->m_vIndex[8] },
			{ m_stModel.m_vObj[0]->m_vIndex[9] },
			{ m_stModel.m_vObj[0]->m_vIndex[10] },
			{ m_stModel.m_vObj[0]->m_vIndex[11] },
			{ m_stModel.m_vObj[0]->m_vIndex[12] },
			{ m_stModel.m_vObj[0]->m_vIndex[13] },
			{ m_stModel.m_vObj[0]->m_vIndex[14] },
			{ m_stModel.m_vObj[0]->m_vIndex[15] },
			{ m_stModel.m_vObj[0]->m_vIndex[16] },
			{ m_stModel.m_vObj[0]->m_vIndex[17] },
			{ m_stModel.m_vObj[0]->m_vIndex[18] },
			{ m_stModel.m_vObj[0]->m_vIndex[19] },
			{ m_stModel.m_vObj[0]->m_vIndex[20] },
			{ m_stModel.m_vObj[0]->m_vIndex[21] },
			{ m_stModel.m_vObj[0]->m_vIndex[22] },
			{ m_stModel.m_vObj[0]->m_vIndex[23] },
			{ m_stModel.m_vObj[0]->m_vIndex[24] },
			{ m_stModel.m_vObj[0]->m_vIndex[25] },
			{ m_stModel.m_vObj[0]->m_vIndex[26] },
			{ m_stModel.m_vObj[0]->m_vIndex[27] },
			{ m_stModel.m_vObj[0]->m_vIndex[28] },
			{ m_stModel.m_vObj[0]->m_vIndex[29] },
			{ m_stModel.m_vObj[0]->m_vIndex[30] },
			{ m_stModel.m_vObj[0]->m_vIndex[31] },
			{ m_stModel.m_vObj[0]->m_vIndex[32] },
			{ m_stModel.m_vObj[0]->m_vIndex[33] },
			{ m_stModel.m_vObj[0]->m_vIndex[34] },
			{ m_stModel.m_vObj[0]->m_vIndex[35] }
		};
		*/
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * m_stModel.m_vObj[0]->m_iFaceCount * 3;        // 36 vertices needed for 12 triangles in a triangle list
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;
		hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
		if (FAILED(hr))
			return hr;

		// Set index buffer
		g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// Set primitive topology
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Create the constant buffer
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
		if (FAILED(hr))
			return hr;

		// Initialize the world matrix
		D3DXMatrixIdentity(&g_World);

		// Initialize the view matrix
		D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -20.0f);
		D3DXVECTOR3 At = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&g_View,&Eye, &At, &Up);

		// Initialize the projection matrix
		D3DXMatrixPerspectiveFovLH(&g_Projection,XM_PIDIV2, m_iWindowWidth / (FLOAT)m_iWindowHeight, 0.01f, 1000.0f);



		delete[] indices;
		delete[] vertices;
		return true; 
	};
	bool		Frame() {
		

		return true; 
	};
	bool		Render() { 
		// Update our time
		static float t = 0.0f;
		if (m_driverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			t += (float)XM_PI * 0.0125f;
		}
		else
		{
			static DWORD dwTimeStart = 0;
			DWORD dwTimeCur = GetTickCount();
			if (dwTimeStart == 0)
				dwTimeStart = dwTimeCur;
			t = (dwTimeCur - dwTimeStart) / 1000.0f;
		}

		//
		// Animate the cube
		//
		D3DXMatrixRotationY(&g_World, t);

		//
		// Clear the back buffer
		//
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
		g_pImmediateContext->ClearRenderTargetView(GetRenderTargetView(), ClearColor);

		//
		// Update variables
		//
		ConstantBuffer cb;
		D3DXMatrixTranspose(&cb.mWorld,&g_World);
		D3DXMatrixTranspose(&cb.mView,&g_View);
		D3DXMatrixTranspose(&cb.mProjection,&g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);

		//
		// Renders a triangle
		//
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
		g_pImmediateContext->DrawIndexed(36, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list

		//
		// Present our back buffer to our front buffer
		//
		m_pSwapChain->Present(0, 0);

		return true;
	};
	bool		Release() { 
		
		if (g_pConstantBuffer) g_pConstantBuffer->Release();
		if (g_pVertexBuffer) g_pVertexBuffer->Release();
		if (g_pIndexBuffer) g_pIndexBuffer->Release();
		if (g_pVertexLayout) g_pVertexLayout->Release();
		if (g_pVertexShader) g_pVertexShader->Release();
		if (g_pPixelShader) g_pPixelShader->Release();

		return true; };
	Sample();
	virtual ~Sample();
};

