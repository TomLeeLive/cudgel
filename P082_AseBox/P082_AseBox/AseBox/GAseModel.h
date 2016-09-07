#pragma once

class GAnimTrack
{
public:	
	int				iTick;		// �ð�(ƽ ����) 
	D3DXQUATERNION  qRotate; ;	// ������ �� �� ���͸� ���Ͽ� ������� ��ȯ
	D3DXVECTOR3		vecVector;	// ��ġ ���� �� ������ ������ Ȱ�� 
	GAnimTrack*		pNext;		// ���� Ʈ�� ����Ʈ �ּ� 
	GAnimTrack*		pPrev;		// ���� Ʈ�� ����Ʈ �ּ� 
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
	// Scene������..
	TCHAR				m_szName[MAX_PATH];				//*SCENE_FILENAME "Box.max"
	int					m_iFrame;						//*SCENE_FIRSTFRAME 0
	int					m_iLastFrame;					//*SCENE_LASTFRAME 100
	int					m_iFrameSpeed;					//*SCENE_FRAMESPEED 30
	int					m_iTicksPerFrame;				//*SCENE_TICKSPERFRAME 160
};

struct GAseMaterial {
	vector<shared_ptr<GAseMaterial>>				m_vSubMaterial;
	//int												m_iSubMaterial;				//0�̸� SubMaterial ���� �ɷ� ó��. 0�� �ƴϸ� Submaterial ����.
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
	D3DXMATRIX						m_matWorld;					//�������

	vector<PNCT_VERTEX>				m_vPnctVertex;				//for VB
	ComPtr<ID3D11Buffer>			m_pVertexBuffer = NULL;
	ComPtr<ID3D11Buffer>			m_pIndexBuffer = NULL;

	//for Animation [START]
	bool							m_bHasAniTrack;				//�ִϸ��̼������� ������ true;
	D3DXMATRIX						m_matWorldTrans;			// �����̵���� 
	D3DXMATRIX						m_matWorldRotate;			// ����ȸ����� 
	D3DXMATRIX						m_matWorldScale;			// ���������� 

	vector<shared_ptr<GAnimTrack>>	m_vPosTrack;				// �̵�Ʈ�� 
	vector<shared_ptr<GAnimTrack>>	m_vRotTrack;				// ȸ��Ʈ�� 
	vector<shared_ptr<GAnimTrack>>	m_vSclTrack;				// ����Ʈ�� 

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