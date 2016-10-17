#include "GObjMgr.h"

int		GObjMgr::Load(ID3D11Device* pd3dDevice,

	const TCHAR* strFileName,
	const TCHAR* strShaderName)
{
	TCHAR szFileName[256];
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];

	// 종복 방지 
	if (strFileName)
	{

		_tsplitpath_s(strFileName, Drive, Dir, FName, Ext);
		Ext[4] = 0;
		_stprintf_s(szFileName, _T("%s%s"), FName, Ext);

		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			GModel *pPoint = (GModel *)(*itor).second;
			if (!_tcsicmp(pPoint->m_szName.c_str(), szFileName))
			{
				return (*itor).first;
			}
		}
	}

	if (GetFileTypeID(strFileName) == NULLFILE || m_pModelObj == NULL)
	{
		return -1;
	}

	m_pModelObj->Init();
	m_pModelObj->m_szName = szFileName;
	m_pModelObj->m_bSetLoadDraw = false;
	if (m_pModelObj->Load(pd3dDevice, strFileName, strShaderName))
	{		
		GMap.insert(make_pair(m_iCurIndex++, m_pModelObj));
		return m_iCurIndex - 1;
	}
	return -1;
};


FILETYPE GObjMgr::GetFileTypeID(const TCHAR* pszFileName)
{
	TCHAR szFileName[256];
	TCHAR Drive[MAX_PATH];
	TCHAR Dir[MAX_PATH];
	TCHAR FName[MAX_PATH];
	TCHAR Ext[MAX_PATH];
	_tsplitpath(pszFileName, Drive, Dir, FName, Ext);
	Ext[4] = 0;
	_stprintf_s(szFileName, _T("%s%s"), FName, Ext);

	if (!_tcsicmp(Ext, _T(".ASE")))
	{
		GModel* pModelObj = NULL;
		SAFE_NEW(pModelObj, GAseObj);
		_ASSERT(pModelObj);
		m_pModelObj = pModelObj;
		return ASEFILE;
	}
	/*if( !_tcsicmp( Ext, _T(".TBS") ) )
	{
	GModel* pModelObj =NULL;
	SAFE_NEW( pModelObj, TbsObj );
	_ASSERT( pModelObj );
	m_pModelObj = pModelObj;
	return TBSFILE;
	}*/
	/*if( !_tcsicmp( Ext, _T(".SKM") ) )
	{
	GModel* pModelObj =NULL;
	SAFE_NEW( pModelObj, TSkinObj );
	_ASSERT( pModelObj );
	m_pModelObj = pModelObj;
	return SKMFILE;
	}*/
	/*if( !_tcsicmp( Ext, _T(".MAT") ) )
	{
	GModel* pModelObj =NULL;
	SAFE_NEW( pModelObj, TBoneObj );
	_ASSERT( pModelObj );
	m_pModelObj = pModelObj;
	return MATFILE;
	}*/
	return NULLFILE;
}

void GObjMgr::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	GTemplateMap< GModel >::TemplateMapItor itor;
	for (itor = GMap.begin(); itor != GMap.end(); itor++)
	{
		GModel *pPoint = (GModel *)(*itor).second;
		if (pPoint)
		{
			pPoint->SetMatrix(pWorld, pView, pProj);
		}
	}
}

GObjMgr::GObjMgr()
{
}

GObjMgr::~GObjMgr()
{
}
