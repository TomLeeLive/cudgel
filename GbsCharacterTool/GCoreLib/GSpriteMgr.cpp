#include "GSpriteMgr.h"

INT GSpriteMgr::Add(ID3D11Device*	 pDevice, 
	TCHAR *pTextureFileName,
	TCHAR *pShaderFileName, 
	ID3D11BlendState* pBlendState,
	bool bInstancing)
{
	TCHAR szFileName[MAX_PATH];
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];
	if (pTextureFileName)
	{
		_tsplitpath_s(pTextureFileName, Drive, Dir, FName, Ext);
		Ext[4] = 0;
		memset(szFileName, 0, sizeof(TCHAR) * MAX_PATH);
		_stprintf_s(szFileName, _T("%s%s"), FName, Ext);

		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			GSprite *pPoint = (GSprite *)(*itor).second;
			if (!_tcsicmp(pPoint->m_szName.c_str(), szFileName))
			{
				return (*itor).first;
			}
		}
	}
	GSprite *pPoint = NULL;
	SAFE_NEW(pPoint, GSprite);
	assert(pPoint);
	pPoint->m_szPath = Dir;

	if (FAILED(pPoint->Load(pDevice, pShaderFileName, pTextureFileName, bInstancing, pBlendState)))
	{
		SAFE_DEL(pPoint);
		return 0;
	}
	GMap.insert(make_pair(++m_iCurIndex, pPoint));
	pPoint->m_iIndex = m_iCurIndex;
	return m_iCurIndex;
}
bool GSpriteMgr::Release()
{
	GSprite *pPoint;
	for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
	{
		pPoint = (GSprite *)(*itor).second;
		pPoint->Release();
		SAFE_DEL(pPoint);
	}
	GMap.clear();
	m_iCurIndex = 0;
	return true;
}
GSprite* GSpriteMgr::GetPtr(INT iIndex)
{
	TemplateMapItor itor = GMap.find(iIndex);
	if (itor == GMap.end()) return NULL;
	GSprite *pPoint = (*itor).second;
	return pPoint;
}
void GSpriteMgr::SetDevice(ID3D11Device*	 pDevice)
{
	m_pd3dDevice = pDevice;
}
GSpriteMgr::GSpriteMgr(void)
{
	m_pd3dDevice = NULL;
	m_iCurIndex = 0;
	GMap.clear();
}

GSpriteMgr::~GSpriteMgr(void)
{
	GSprite *pPoint;
	for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
	{
		pPoint = (GSprite *)(*itor).second;
		pPoint->Release();
		SAFE_DEL(pPoint);
	}
	GMap.clear();
	m_iCurIndex = 0;
}
