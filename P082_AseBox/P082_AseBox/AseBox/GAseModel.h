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

struct GAseScene {
	// Scene정보들..
	TCHAR				m_szName[MAX_PATH];				//*SCENE_FILENAME "Box.max"
	int					m_iFrame;						//*SCENE_FIRSTFRAME 0
	int					m_iLastFrame;					//*SCENE_LASTFRAME 100
	int					m_iFrameSpeed;					//*SCENE_FRAMESPEED 30
	int					m_iTicksPerFrame;				//*SCENE_TICKSPERFRAME 160
};

struct GAseMaterial {
	vector<shared_ptr<GAseMaterial>>				m_vSubMaterial;
	//int												m_iSubMaterial;				//0이면 SubMaterial 없는 걸로 처리. 0이 아니면 Submaterial 있음.
	TCHAR											m_szName[MAX_PATH];			//*MATERIAL_NAME "01 - Default"
	D3DXVECTOR3										m_vecAmbient;				//*MATERIAL_AMBIENT 0.5882	0.5882	0.5882
	D3DXVECTOR3										m_vecDiffuse;				//*MATERIAL_DIFFUSE 0.5882	0.5882	0.5882
	D3DXVECTOR3										m_vecSpecular;				//*MATERIAL_SPECULAR 0.9000	0.9000	0.9000
	TCHAR											m_szMapDiffuse[MAX_PATH];	//*BITMAP "C:\TBasis200\Data\object\textures\flagstone.bmp"
	ComPtr<ID3D11ShaderResourceView>				m_pTextureRV = NULL;
	GAseMaterial() { //m_iSubMaterial = 0; 
	};
	~GAseMaterial() {};
};

class GAseObj {
public:
	TCHAR							m_szName[MAX_PATH];			//*NODE_NAME "Box01"
	D3DXMATRIX						m_matWorld;					//월드행렬

	vector<PNCT_VERTEX>				m_vPnctVertex;				//for VB
	ComPtr<ID3D11Buffer>			m_pVertexBuffer = NULL;
	ComPtr<ID3D11Buffer>			m_pIndexBuffer = NULL;

	//for Animation [START]
	bool							m_bHasAniTrack;				//애니메이션정보가 있으면 true;
	D3DXMATRIX						m_matWorldTrans;			// 월드이동행렬 
	D3DXMATRIX						m_matWorldRotate;			// 월드회전행렬 
	D3DXMATRIX						m_matWorldScale;			// 월드신축행렬 

	vector<shared_ptr<GAnimTrack>>	m_vPosTrack;				// 이동트랙 
	vector<shared_ptr<GAnimTrack>>	m_vRotTrack;				// 회전트랙 
	vector<shared_ptr<GAnimTrack>>	m_vSclTrack;				// 신축트랙 

	D3DXQUATERNION					m_qRotation;				

	D3DXVECTOR3						m_vecTM_POS;				
	D3DXVECTOR3						m_vecTM_ROTAXIS;			
	float							m_fTM_ROTANGLE;				
	D3DXVECTOR3						m_vecTM_SCALE;				
	D3DXVECTOR3						m_vecTM_SCALE_AXIS;			
	float							m_fTM_SCALEAXISANG;			
	//for Animation [END]
	
	GAseObj() { 
		D3DXMatrixIdentity(&m_matWorldTrans);
		D3DXMatrixIdentity(&m_matWorldRotate);
		D3DXMatrixIdentity(&m_matWorldScale);

		m_bHasAniTrack = false;
	};
	~GAseObj() {
		m_vPnctVertex.clear();
	};

};

class GAseModel {
public:
	float									m_fLastFrame;
	float									m_fTickFrame;
	float									m_fFrameSpeed;
	float									m_fTickPerFrame;

	GAseScene								m_stScene;
	vector<shared_ptr<GAseMaterial>>		m_vMaterial;
	vector<shared_ptr<GAseObj>>				m_vObj;

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

	void		AniFrame();
	void		GetAnimationTrack(float fCurrentTick, GAnimTrack** pStartTrack, GAnimTrack** pEndTrack, ANITRACK_TYPE nTrackType);
	bool		Init(TCHAR* strFileName, TCHAR* strShaderName);
	bool		Frame();
	bool		Render(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj);
	bool		Release();
public:
	GAseModel() {
		m_fLastFrame	= 0.0f;
		m_fTickFrame	= 0.0f;
		m_fFrameSpeed	= 0.0f;
		m_fTickPerFrame = 0.0f;
	};
	~GAseModel() {};
};