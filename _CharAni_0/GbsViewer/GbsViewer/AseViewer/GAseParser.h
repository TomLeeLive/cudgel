#pragma once

#ifndef __GASEPARSER_H__
#define __GASEPARSER_H__

struct GAnimTrack;
struct GAseScene;
struct GAseMaterial;

class GAseObj;
class GAseModel;


class GAseParser : public GParser, public GSingleton < GAseParser >
{
public:
	TCHAR* g_pAseItems[4] = { 
		L"*SCENE",
		L"*MATERIAL_LIST",
		L"*GEOMOBJECT",
		L"*HELPEROBJECT" };
	enum AseItemsType { 
		SCENE = 0, 
		MATERIALLIST, 
		GEOMOBJECT, 
		HELPEROBJECT };

	TCHAR* g_pAseSceneTokens[5] = { 
		L"*SCENE_FILENAME", 
		L"*SCENE_FIRSTFRAME",
		L"*SCENE_LASTFRAME",
		L"*SCENE_FRAMESPEED",
		L"*SCENE_TICKSPERFRAME" };

	TCHAR* g_pAseMaterialTokens[6] = { 
		L"*MATERIAL_NAME",
		L"*MATERIAL_AMBIENT",
		L"*MATERIAL_DIFFUSE", 
		L"*MATERIAL_SPECULAR",
		L"*MAP_SUBNO",
		L"*BITMAP"
	};

	TCHAR* g_pAseGeomObjectTokens[8] = { 
		L"*NODE_PARENT",
		L"*NODE_NAME",
		L"*NODE_TM",
		L"*MESH",
		L"*TM_ANIMATION",
		L"*MATERIAL_REF",
		L"*BOUNDINGBOX_MIN",
		L"*BOUNDINGBOX_MAX"
	};

	enum AseGeomObjectTokensType { 
		NODE_PARENT = 0,
		NODE_NAME, 
		NODE_TM, 
		MESH, 
		TM_ANIMATION,
		MATERIAL_REF,
		BOUNDINGBOX_MIN,	//Helperobject의 경우 bounding box가 있다.
		BOUNDINGBOX_MAX
	};

	TCHAR* g_pAseNodeTmTokens[12] = { 
		L"*TM_ROW0",
		L"*TM_ROW1", 
		L"*TM_ROW2", 
		L"*TM_ROW3", 
		L"*TM_POS",
		L"*TM_ROTAXIS",
		L"*TM_ROTANGLE",
		L"*TM_SCALE",
		L"*TM_SCALEAXIS",
		L"*TM_SCALEAXISANG", 
	};

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

	TCHAR* g_pAseAniDataTokens[10] = {
		L"*NODE_NAME",
		L"*CONTROL_POS_TRACK",
		L"*CONTROL_POS_TCB",
		L"*CONTROL_POS_BEZIER",
		L"*CONTROL_ROT_TRACK",
		L"*CONTROL_ROT_TCB",
		L"*CONTROL_ROT_BEZIER",
		L"*CONTROL_SCALE_TRACK",
		L"*CONTROL_SCALE_TCB",
		L"*CONTROL_SCALE_BEZIER"
	};

	enum AseAniDataTokensType {
		ANI_NODE_NAME,
		ANI_CONTROL_POS_TRACK,
		ANI_CONTROL_POS_TCB,
		ANI_CONTROL_POS_BEZIER,
		ANI_CONTROL_ROT_TRACK,
		ANI_CONTROL_ROT_TCB,
		ANI_CONTROL_ROT_BEZIER,
		ANI_CONTROL_SCALE_TRACK,
		ANI_CONTROL_SCALE_TCB,
		ANI_CONTROL_SCALE_BEZIER
	};

	TCHAR* g_pAseAniTrackTokens[3] = {
		L"*CONTROL_POS_SAMPLE",
		L"*CONTROL_ROT_SAMPLE",
		L"*CONTROL_SCALE_SAMPLE"
	};

	enum AseAniTrackTokensType {
		ANI_CONTROL_POS_SAMPLE,
		ANI_CONTROL_ROT_SAMPLE,
		ANI_CONTROL_SCALE_SAMPLE
	};
	
	enum AseTrackType {
		POS_SAMPLE_TRACK = 40, 
		ROT_SAMPLE_TRACK, 
		SCL_SAMPLE_TRACK, 
		VIS_SAMPLE_TRACK
	};
	
	int			m_iObjCount;
	fpos_t		m_FilePosition;
	
	vector<D3DXVECTOR3>				m_vPosList;					// P
	vector<D3DXVECTOR3>				m_vNorList;					// N
	vector<D3DXVECTOR3>				m_vColList;					// C
	vector<D3DXVECTOR3>				m_vTexList;					// T

	vector<D3DXVECTOR3>				m_vFaceNormal;				// Face Normal;
	vector<int>						m_vIndex;					//for IB
	vector<int>						m_vTextureIndex;			//for Texture Index
	vector<int>						m_vSubMtlIndex;

	int								m_iPosCount;				//Vertex 카운트
	int								m_iFaceCount;				//Face 카운트
	int								m_iTexVerCount;				//Texture vertex 카운트   MESH_NUMTVERTEX
	int								m_iTexFaceCount;			//Texture Face 카운트     MESH_NUMTVFACES
	int								m_iColorVerCount;			//MESH_NUMCVERTEX

	void	ResetVariables() {
		

		m_vPosList.clear();
		m_vNorList.clear();
		m_vColList.clear();
		m_vTexList.clear();
		m_vFaceNormal.clear();
		m_vIndex.clear();
		m_vTextureIndex.clear();
		m_vSubMtlIndex.clear();

		//m_iObjCount		 = 0;
		m_iPosCount      = 0;
		m_iFaceCount	 = 0;
		m_iTexVerCount	 = 0;
		m_iTexFaceCount  = 0;
		m_iColorVerCount = 0;
	}

	void	CountGeomObjFromFile(GAseModel* stModel);
	void	InitAseModel(TCHAR* strFile, GAseModel* stModel);
	//void	SetPnctMultiObjData(GAseModel* stModel,int iObjNum = 0);
	void    SetPnctData(GAseModel* stModel, int iObjNum = 0);
	bool	GetTrackListFromString(GAseModel* stModel /*vector<shared_ptr<GAnimTrack>>& vTrack*/, AseTrackType TrackType);
	int		GetAnimationDataFromFile(GAseModel* stModel);
	int		GetMeshDataFromFile(GAseModel* stModel);
	//int		GetHelperObjDataFromFile(GAseModel* stModel);
	int		GetGeomObjDataFromFile(GAseModel* stModel);
	int		GetDataFromFile(GAseModel* stModel);
	void	GetStringWeNeed(VOID* pOutStr, VOID* pInStr);

	void ProcessInheritanceBtwObjs(GAseModel* stModel);
	void SaveFilePosition()
	{
		if (fgetpos(m_pStream, &m_FilePosition))
		{
			CloseStream();
		}
	}
	void RestoreFilePosition()
	{
		if (fsetpos(m_pStream, &m_FilePosition))
		{
			CloseStream();
		}
	}

	GAseParser();
	virtual ~GAseParser();
};

#define I_AseParser GAseParser::GetInstance()

#endif
