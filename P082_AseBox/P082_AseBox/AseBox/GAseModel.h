#pragma once


struct ConstantBuffer
{
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
};

class GAseObj {
public:

	ID3D11ShaderResourceView*           m_pTextureRV = NULL;

	ID3D11Buffer*			m_pVertexBuffer = NULL;
	ID3D11Buffer*			m_pIndexBuffer = NULL;

	TCHAR					m_szName[MAX_PATH];			//*NODE_NAME "Box01"
	D3DXMATRIX				m_matWorld;					//�������
	int						m_iPosCount;				//Vertex ī��Ʈ
	int						m_iFaceCount;				//Face ī��Ʈ
	int						m_iTexVerCount;				//Texture vertex ī��Ʈ   MESH_NUMTVERTEX
	int						m_iTexFaceCount;			//Texture Face ī��Ʈ     MESH_NUMTVFACES
	int						m_iColorVerCount;			//MESH_NUMCVERTEX

	vector<D3DXVECTOR3>		m_vPosList;		// P
	vector<D3DXVECTOR3>		m_vNorList;		// N
	vector<D3DXVECTOR3>		m_vColList;		// C
	vector<D3DXVECTOR3>		m_vTexList;		// T

	vector<D3DXVECTOR3>		m_vFaceNormal;	// Face Normal;

	vector<PNCT_VERTEX>		m_vPnctVertex;			//VB
	vector<int>				m_vIndex;			//for IB
	vector<int>				m_vTextureIndex;	//for Texture Index

	GAseObj() {};
	~GAseObj() {};

};

class GAseModel {
public:
	GAseScene								m_stScene;
	vector<shared_ptr<GAseMaterial>>		m_vMaterial;
	vector<shared_ptr<GAseObj>>				m_vObj;

	ID3D11VertexShader*					m_pVertexShader = NULL;
	ID3D11PixelShader*					m_pPixelShader = NULL;
	ID3D11InputLayout*					m_pVertexLayout = NULL;
	ID3D11Buffer*						m_pConstantBuffer = NULL;

	ID3D11SamplerState*                 m_pSamplerLinear = NULL;

	bool		Init();
	bool		Frame(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj);
	bool		Render();
	bool		Release();
public:
	GAseModel() {};
	~GAseModel() {};
};