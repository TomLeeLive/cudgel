#pragma once

class GAnimTrack
{
public:	
	int				iTick;		// 시간(틱 단위) 
	D3DXQUATERNION  qRotate; ;	// 임의의 축 및 벡터를 통하여 사원수로 변환
	D3DXVECTOR3		vecVector;	// 위치 벡터 및 스케일 값으로 활용 
	GAnimTrack*		pNext;		// 다음 트랙 리스트 주소 
	GAnimTrack*		pPrev;		// 이전 트랙 리스트 주소 
	GAnimTrack() {
		pNext = NULL;
		pPrev = NULL;
	}
	~GAnimTrack() {

	}
};

struct ConstantBuffer
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

class GGbsScene {
public:
	// Scene정보들..
	//TCHAR				m_szName[MAX_PATH];				//*SCENE_FILENAME "Box.max"
	//int					m_iFrame;						//*SCENE_FIRSTFRAME 0
	//int					m_iLastFrame;					//*SCENE_LASTFRAME 100
	//int					m_iFrameSpeed;					//*SCENE_FRAMESPEED 30
	//int					m_iTicksPerFrame;				//*SCENE_TICKSPERFRAME 160
	int iVersion;		// 버전
	int	iFirstFrame;    // 시작 프레임
	int	iLastFrame;     // 마지막 프레임
	int	iFrameSpeed;    // 1초당 프레임 개수(30)
	int	iTickPerFrame;  // 1프레임의 틱 값(160)
	int iNumMesh;		// 메쉬오브젝트 개수
	int iMaxWeight;		// 정점 당 가중치
	int iBindPose;		// 바인딩 포즈 에니메이션 여부

	GGbsScene() {
		//memset(m_szName, 0, sizeof(m_szName));
	}
	~GGbsScene() {}
};

class GGbsMaterial {
public:
	//for GBS 추가함 [START]
	DWORD m_dwSubCount;
	DWORD m_dwTexMapCount;
	DWORD m_dwIndex;
	//for GBS 추가함 [END]

//	int	m_iType;		// 1: diffuse 9:reflect
	bool  m_iDiffuse;
	bool  m_iReflect;

	vector<shared_ptr<GGbsMaterial>>				m_vSubMaterial;
	//int												m_iSubMaterial;				//0이면 SubMaterial 없는 걸로 처리. 0이 아니면 Submaterial 있음.
	TCHAR											m_szName[MAX_PATH];			//*MATERIAL_NAME "01 - Default"
	TCHAR											m_szClsName[MAX_PATH];
	D3DXVECTOR3										m_vecAmbient;				//*MATERIAL_AMBIENT 0.5882	0.5882	0.5882
	D3DXVECTOR3										m_vecDiffuse;				//*MATERIAL_DIFFUSE 0.5882	0.5882	0.5882
	D3DXVECTOR3										m_vecSpecular;				//*MATERIAL_SPECULAR 0.9000	0.9000	0.9000
	TCHAR											m_szMapDiffuse[MAX_PATH];	//*BITMAP "C:\TBasis200\Data\object\textures\flagstone.bmp"
	TCHAR											m_szMapReflect[MAX_PATH];
	ComPtr<ID3D11ShaderResourceView>				m_pTextureRV = NULL;
	GGbsMaterial() { //m_iSubMaterial = 0; 
		m_iDiffuse = 0;
		m_iReflect = 0;
//		m_iType = -1;
		memset(m_szName, 0, sizeof(m_szName));
		memset(m_szClsName, 0, sizeof(m_szClsName));
		memset(m_szMapDiffuse, 0, sizeof(m_szMapDiffuse));
		memset(m_szMapReflect, 0, sizeof(m_szMapReflect));
	};
	~GGbsMaterial() {};
};

struct GTri
{
	PNCT_VERTEX		vVertex[3];
	D3DXVECTOR3  vNormal;
	int						iSubIndex;
	GTri(int Index) : iSubIndex(Index) {}
	GTri() : iSubIndex(-1) {}
};


class GGbsObj {
public:

	//추가함 for GBS [START]
	int					m_iNumFace;
	vector<GTri>		m_TriList;
	vector<shared_ptr<GGbsObj>>		m_pSubMesh;
	//추가함 for GBS [END]

	vector<DWORD>					m_vIndexList;
	vector<PNCT_VERTEX>				m_vPnctVertex;				//for VB
	ComPtr<ID3D11Buffer>			m_pVertexBuffer = NULL;
	ComPtr<ID3D11Buffer>			m_pIndexBuffer = NULL;

	GGbsObj() {};
	~GGbsObj() { m_vPnctVertex.clear();};

};
typedef vector<shared_ptr<GGbsObj>>	gGbsMeshData;

class GGbsGeom {
public:
	gGbsMeshData							m_pData;
	//추가함 for GBS [START]
	int										m_iIndex;
	OBJECTCLASSTYPE							m_ClassType;
	int										m_iNumFace;
	int										m_iDiffuseTex;
	D3DXMATRIX								m_matInverse;
	G_BOX									m_BBox;
	float									m_fVisibility;
	int										m_iNumTrack[4];
	int										m_iMtrlRef;
	//추가함 for GBS [END]

	D3DXMATRIX								m_matWld;					//월드행렬
	int										m_iType;					//0:Geom 1:Helper
	TCHAR									m_szName[MAX_PATH];			//*NODE_NAME "Box01"
	TCHAR									m_szParentName[MAX_PATH];	//*NODE_PARENT "Dummy01"
	int										m_iMaterial_Ref;
	bool									m_bUsed;					//렌더링 제외여부. false일 경우 렌더링 제외한다.
	D3DXMATRIX								m_matChlWld;				//부모월드행렬의 역행렬을 곱한 자식월드 행렬.
	D3DXMATRIX								m_matCalculation;			//계산된 최종행렬
	D3DXVECTOR3								m_vecBoundingboxMin;
	D3DXVECTOR3								m_vecBoundingboxMax;


	GGbsGeom*								m_pParentObj;
	vector<GGbsGeom*>  						m_pChildObj;

	vector<shared_ptr<GGbsGeom>>			m_vSubObj;
	vector<shared_ptr<GGbsObj>>				m_vSubObjData;


	//for Animation [START]
	bool							m_bHasAniTrack;				// 애니메이션정보가 있으면 true;
	D3DXMATRIX						m_matWldTrans;			// 월드이동행렬 
	D3DXMATRIX						m_matWldRotate;			// 월드회전행렬 
	D3DXMATRIX						m_matWldScale;			// 월드신축행렬 

	vector<shared_ptr<GAnimTrack>>	m_vPosTrack;				// 이동트랙 
	vector<shared_ptr<GAnimTrack>>	m_vRotTrack;				// 회전트랙 
	vector<shared_ptr<GAnimTrack>>	m_vSclTrack;				// 신축트랙 
	vector<shared_ptr<GAnimTrack>>	m_vVisTrack;				// ? 

	D3DXQUATERNION					m_qRotation;

	D3DXVECTOR3						m_vecTM_POS;
	D3DXVECTOR3						m_vecTM_ROTAXIS;
	float							m_fTM_ROTANGLE;
	D3DXVECTOR3						m_vecTM_SCALE;
	D3DXVECTOR3						m_vecTM_SCALE_AXIS;
	float							m_fTM_SCALEAXISANG;
	//for Animation [END]
	GGbsGeom(){
		m_iIndex = -1;
		m_iNumFace = -1;
		m_iDiffuseTex = -1;
		m_fVisibility = 0.0f;
		m_iMtrlRef = -1;

		m_pParentObj = NULL;

		m_bUsed = true;
		m_iMaterial_Ref = -1;
		memset(m_szName, 0, sizeof(m_szName));
		memset(m_szParentName, 0, sizeof(m_szParentName));

		D3DXMatrixIdentity(&m_matInverse);
		D3DXMatrixIdentity(&m_matWld);
		D3DXMatrixIdentity(&m_matChlWld);
		D3DXMatrixIdentity(&m_matWldTrans);
		D3DXMatrixIdentity(&m_matWldRotate);
		D3DXMatrixIdentity(&m_matWldScale);
		D3DXMatrixIdentity(&m_matCalculation);

		m_bHasAniTrack = false;

		m_iType = 0;
	}
	~GGbsGeom(){}

};

class GGbsModel {
public:


	float									m_fLastFrame;
	float									m_fTickFrame;
	float									m_fFrameSpeed;
	float									m_fTickPerFrame;

	GGbsScene								m_stScene;
	vector<shared_ptr<GGbsMaterial>>		m_vMaterial;
	vector<shared_ptr<GGbsGeom>>			m_vGeomObj;

	ComPtr<ID3D11Buffer>					m_pConstantBuffer = NULL;
	ComPtr<ID3D11VertexShader>				m_pVertexShader = NULL;
	ComPtr<ID3D11PixelShader>				m_pPixelShader = NULL;
	ComPtr<ID3D11InputLayout>				m_pVertexLayout = NULL;
	ComPtr<ID3D11SamplerState>				m_pSamplerLinear = NULL;

	enum ANITRACK_TYPE {
		ANITRACK_TYPE_POS = 0,
		ANITRACK_TYPE_ROT,
		ANITRACK_TYPE_SCL
	};

	bool		SingleModelInit();
	void		SingleAniFrame();
	bool		MultiModelInit();
	void		MultiAniFrame();
	void		GetAnimationTrack(float fCurrentTick, GAnimTrack** pStartTrack, GAnimTrack** pEndTrack, ANITRACK_TYPE nTrackType,int iGeomNum = 0);
	bool		Init(TCHAR* strFileName, TCHAR* strShaderName);
	bool		Frame();
	bool		SingleRender(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj);
	bool		MultiRender(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj);
	bool		Render(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj);
	bool		Release();
public:
	GGbsModel() {
		m_fLastFrame	= 0.0f;
		m_fTickFrame	= 0.0f;
		m_fFrameSpeed	= 0.0f;
		m_fTickPerFrame = 0.0f;
	};
	~GGbsModel() {};
};