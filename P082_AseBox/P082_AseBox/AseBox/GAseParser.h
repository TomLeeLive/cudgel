#pragma once

#ifndef __GASEPARSER_H__
#define __GASEPARSER_H__

class GAseObj;
class GAseModel;

struct GAseScene {
	// Scene¡§∫∏µÈ..
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

class GAseParser : public GParser
{
public:
	TCHAR* g_pAseItems[3] = { L"*SCENE",L"*MATERIAL_LIST",L"*GEOMOBJECT"/*,L"HELPEROBJECT"*/ };
	enum AseItemsType { SCENE = 0, MATERIALLIST, GEOMOBJECT, /*HELPEROBJECT, */ };

	TCHAR* g_pAseSceneTokens[5] = { L"*SCENE_FILENAME", L"*SCENE_FIRSTFRAME", L"*SCENE_LASTFRAME",L"*SCENE_FRAMESPEED",L"*SCENE_TICKSPERFRAME" };
	TCHAR* g_pAseMaterialTokens[5] = { L"*MATERIAL_NAME",L"*MATERIAL_AMBIENT", L"*MATERIAL_DIFFUSE", L"*MATERIAL_SPECULAR",L"*BITMAP" };

	TCHAR* g_pAseObjTokens[3] = { L"*NODE_NAME",L"*NODE_TM",L"*MESH" };
	enum AseObjTokensType { NODE_NAME = 0, NODE_TM, MESH };

	TCHAR* g_pAseNodeTmTokens[4] = { L"*TM_ROW0",L"*TM_ROW1", L"*TM_ROW2", L"*TM_ROW3" };

	TCHAR* g_pAseMeshTokens[11] = { 
		L"*MESH_NUMVERTEX",
		L"*MESH_NUMFACES", 
		L"*MESH_VERTEX_LIST", 
		L"*MESH_FACE_LIST",
		L"*MESH_NUMTVERTEX", 
		L"*MESH_TVERTLIST", 
		L"*MESH_NUMTVFACES",
		L"*MESH_TFACELIST", 
		L"*MESH_NUMCVERTEX", 
		L"*MESH_CVERTLIST",
		L"*MESH_NORMALS" };
	
	enum AseMeshTokensType {
		MESH_NUMVERTEX = 0,	//MESH_NUMVERTEX 8
		MESH_NUMFACES,		//MESH_NUMFACES 12
		MESH_VERTEX_LIST,
		MESH_FACE_LIST,
		MESH_NUMTVERTEX,	//MESH_NUMTVERTEX 12
		MESH_TVERTLIST,
		MESH_NUMTVFACES,	//MESH_NUMTVFACES 12
		MESH_TFACELIST,
		MESH_NUMCVERTEX,	//MESH_NUMCVERTEX 0
		MESH_CVERTLIST,		//*MESH_CVERTLIST
		MESH_NORMALS
	};

	void    SetPnctData(GAseModel* stModel);
	int		GetMeshDataFromFile(GAseModel* stModel,GAseObj* aseobj);
	int		GetObjDataFromFile(GAseModel* stModel);
	int		GetDataFromFile(GAseModel* stModel);

	GAseParser();
	virtual ~GAseParser();
};

#endif