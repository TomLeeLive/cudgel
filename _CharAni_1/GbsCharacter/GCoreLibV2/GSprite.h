#pragma once
#include "GShape.h"

struct GRectUV
{
	D3DXVECTOR4	vUV;
	RECT		Rect;
};
struct GInstatnce
{
	D3DXMATRIX matWorld;
	D3DXVECTOR4  uv[4];
	D3DXVECTOR4  color;
};
class GSprite : public GPlaneShape
{
public:
	T_STR					m_szName;
	T_STR					m_szPath;
	UINT					m_iIndex;
	UINT					m_BlendType;
	D3DXMATRIX				m_matBillboard;
	ID3D11BlendState*			m_pBlendState;
	ID3D11ShaderResourceView*	m_pAnimSRV;

	vector<int>				m_TextureIndex;
	int						m_iApplyIndex;
	int						m_iNumTexture;
	float					m_fSecPerRender;
	float					m_fTime;
	float					m_fLifeTime;
	float					m_fAnimTime; // uv  �ݺ� �ð�
	float					m_fElapseTime;
	RECT					m_RectSet;
	vector<GRectUV>			m_RectList;
	D3DXVECTOR4				m_ParticleColor;
	
	vector<GInstatnce>			m_pInstance;
	ComPtr<ID3D11Buffer>		m_pVBInstance;
	bool						m_bInstancing;
public:
	HRESULT		SetInputLayout();
	HRESULT		CreateInstance(UINT iNumInstance);
	HRESULT		Load(	ID3D11Device* pDevice,
						TCHAR* pLoadTextureString,
						TCHAR* pLoadShaderString,
						bool   bInstancing = false,
						ID3D11BlendState* pBlendState = nullptr);
	bool		Frame(ID3D11DeviceContext*    pContext, float fGlobalTime, float fElapsedTime);
	void		Updata(ID3D11DeviceContext*    pContext, float& pfCurrentTimer, int& iApplyIndex, float fGlobalTime, float fElapsedTime);
	bool		PreDraw(ID3D11DeviceContext* pContext);
	bool		Render(ID3D11DeviceContext* pContext);
	bool		PostDraw(ID3D11DeviceContext* pContext);
	bool		RenderInstancing(ID3D11DeviceContext* pContext);
	void		SetTextureArray(T_STR_VECTOR FileList);
	D3DXVECTOR4	SetUV(RECT& Rect);
	void		SetUVAnimation(float fAnimTime,int iWidth=1,int iHeight=1);
	void		SetRectAnimation(
		float fAnimTime = 1.0f, 
		int iWidth = 1, 
		int iWidthSize = 0, 
		int iHeight = 1, 
		int iHeightSize = 0);
public:
	GSprite(void);
	~GSprite(void);
};

