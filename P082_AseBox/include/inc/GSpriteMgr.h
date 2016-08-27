#pragma once
#include "GSprite.h"

class GSpriteMgr : public GSingleton < GSpriteMgr >
{
private:
	friend class GSingleton<GSpriteMgr>;
	ID3D11Device*			m_pd3dDevice;
public:
	typedef map <INT, GSprite*>					TemplateMap;
	typedef TemplateMap::iterator				TemplateMapItor;
	TemplateMapItor								TItor;
	TemplateMap									GMap;
	INT											m_iCurIndex;
public:
	void			SetDevice(ID3D11Device*	 pDevice);
	INT				Add(ID3D11Device*	 pDevice,
						TCHAR *pTextureFileName,
						TCHAR *pShaderFileName,						
						ID3D11BlendState* m_pBlendState = nullptr,
						bool bInstancing = false );
	GSprite*		GetPtr(INT iIndex);
	bool			Release();
	bool			Delete(INT iDelete);
public:
	GSpriteMgr();
	~GSpriteMgr();
};
#define I_Sprite GSpriteMgr::GetInstance()