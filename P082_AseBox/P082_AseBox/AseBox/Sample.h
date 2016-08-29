#pragma once
#include <GBasisLib_0.h>
#include <GShape.h>
#include "GParser.h"


ID3D11VertexShader*     g_pVertexShader = NULL;
ID3D11PixelShader*      g_pPixelShader = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3D11Buffer*           g_pVertexBuffer = NULL;
ID3D11Buffer*           g_pIndexBuffer = NULL;
ID3D11Buffer*           g_pConstantBuffer = NULL;
D3DXMATRIX                g_World;
D3DXMATRIX                g_View;
D3DXMATRIX                g_Projection;

ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11SamplerState*                 g_pSamplerLinear = NULL;


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

TCHAR* g_pAseMeshTokens[] = { L"*MESH_NUMVERTEX",L"*MESH_NUMFACES", L"*MESH_VERTEX_LIST", L"*MESH_FACE_LIST",L"*MESH_NUMTVERTEX", L"*MESH_TVERTLIST", L"*MESH_NUMTVFACES",L"*MESH_TFACELIST", L"*MESH_NUMCVERTEX", L"*MESH_NORMALS" };
enum AseMeshTokensType {	 
	MESH_NUMVERTEX =0,	//MESH_NUMVERTEX 8
	MESH_NUMFACES,		//MESH_NUMFACES 12
	MESH_VERTEX_LIST, 
	MESH_FACE_LIST, 
	MESH_NUMTVERTEX,	//MESH_NUMTVERTEX 12
	MESH_TVERTLIST, 
	MESH_NUMTVFACES,	//MESH_NUMTVFACES 12
	MESH_TFACELIST, 
	MESH_NUMCVERTEX,	//MESH_NUMCVERTEX 0
	MESH_NORMALS };

struct GAseScene {
	// Scene정보들..
	TCHAR				m_szName[MAX_PATH];				//*SCENE_FILENAME "Box.max"
	int					m_iFrame;						//*SCENE_FIRSTFRAME 0
	int					m_iLastFrame;					//*SCENE_LASTFRAME 100
	int					m_iFrameSpeed;					//*SCENE_FRAMESPEED 30
	int					m_iTicksPerFrame;				//*SCENE_TICKSPERFRAME 160
};

struct GAseMaterial {
	TCHAR					m_szName[MAX_PATH];			//*MATERIAL_NAME "01 - Default"
	D3DXVECTOR3				m_vecAmbient;				//*MATERIAL_AMBIENT 0.5882	0.5882	0.5882
	D3DXVECTOR3				m_vecDiffuse;				//*MATERIAL_DIFFUSE 0.5882	0.5882	0.5882
	D3DXVECTOR3				m_vecSpecular;				//*MATERIAL_SPECULAR 0.9000	0.9000	0.9000
	TCHAR					m_szMapDiffuse[MAX_PATH];	//*BITMAP "C:\TBasis200\Data\object\textures\flagstone.bmp"
	GAseMaterial() {};
	~GAseMaterial() {};
};
struct GAseObj {
	TCHAR					m_szName[MAX_PATH];			//*NODE_NAME "Box01"
	D3DXMATRIX				m_matWorld;					//월드행렬
	int						m_iPosCount;				//Vertex 카운트
	int						m_iFaceCount;				//Face 카운트
	int						m_iTexVerCount;				//Texture vertex 카운트   MESH_NUMTVERTEX
	int						m_iTexFaceCount;			//Texture Face 카운트     MESH_NUMTVFACES
	int						m_iColorVerCount;			//MESH_NUMCVERTEX

	vector<D3DXVECTOR3>		m_vPosList;		// P
	vector<D3DXVECTOR3>		m_vNorList;		// N
	vector<D3DXVECTOR3>		m_vColList;		// C
	vector<D3DXVECTOR3>		m_vTexList;		// T

	vector<D3DXVECTOR3>		m_vFaceNormal;	// Face Normal;

	vector<PNCT_VERTEX>		m_vPnctVertex;			//VB
	vector<int>				m_vIndex;			//for IB
	vector<int>				m_vTextureIndex;	//for Texture Index
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

	void    SetPnctData();
	int		GetMeshDataFromFile(GAseObj* aseobj);
	int		GetObjDataFromFile();
	int		GetDataFromFile();
	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();
	Sample();
	virtual ~Sample();
};

