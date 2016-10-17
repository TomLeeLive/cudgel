#pragma once
#include "GBasisStd.h"

class GTexture
{
public:
	T_STR		m_szName;
	T_STR		m_szPath;
	//--------------------------------------------------------------------------------------
	// 텍스쳐 로딩 및 리소스버퍼 생성
	//--------------------------------------------------------------------------------------
	ID3D11ShaderResourceView*	m_pTextureRV;
	ID3D11SamplerState*			m_pSamplerState;
	D3D11_SAMPLER_DESC			m_SampleDesc;
public:
	HRESULT						Load(ID3D11Device* pDevice, const TCHAR* strFilePath);
	bool						Apply(ID3D11DeviceContext*    pImmediateContext);
	bool						Release();
	void						SetPath(const TCHAR* pPath);

public:
	GTexture(void);
	virtual ~GTexture(void);
};

class GTextureMgr : public GSingleton < GTextureMgr >
{
private:
	friend class GSingleton<GTextureMgr>;
	ID3D11Device*			m_pd3dDevice;
	ID3D11DeviceContext*    m_pImmediateContext;
public:
	typedef unordered_map <INT, GTexture*>		TemplateMap;
	typedef TemplateMap::iterator				TemplateMapItor;
	TemplateMapItor								TItor;
	TemplateMap									TMap;
	INT											m_iCurIndex;
public:
	INT				Add(ID3D11Device*	 pDevice, const TCHAR *pFileName);
	INT				Add(ID3D11Device*	 pDevice, const TCHAR *pFileName, const TCHAR* szPath);
	GTexture* const	GetPtr(INT iIndex);
	GTexture* const	GetPtr(T_STR strFindName);
	bool			Release();
public:
	GTextureMgr();
	virtual ~GTextureMgr();
};
#define I_Texture GTextureMgr::GetInstance()

