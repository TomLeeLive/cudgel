#include "GObjMgr.h"

int		GObjMgr::Load(ID3D11Device* pd3dDevice,	const TCHAR* strFileName,
	const TCHAR* strShaderName, bool bThread)
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

		for (TemplateMapItor itor = TMap.begin(); itor != TMap.end(); itor++)
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
	if (m_pModelObj->Load(pd3dDevice, strFileName, strShaderName, bThread))
	{		
		TMap.insert(make_pair(m_iCurIndex++, m_pModelObj));
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
		pModelObj->m_ObjType = ASEFILE;
		return ASEFILE;
	}
	if( !_tcsicmp( Ext, _T(".GBS") ) )
	{
		GModel* pModelObj =NULL;
		SAFE_NEW( pModelObj, GGbsObj );
		_ASSERT( pModelObj );
		m_pModelObj = pModelObj;
		pModelObj->m_ObjType = GBSFILE;
		return GBSFILE;
	}
	if( !_tcsicmp( Ext, _T(".GSK") ) )
	{
		GModel* pModelObj =NULL;
		SAFE_NEW( pModelObj, GSkinObj );
		_ASSERT( pModelObj );
		m_pModelObj = pModelObj;
		pModelObj->m_ObjType = GSKFILE;
		return GSKFILE;
	}
	if( !_tcsicmp( Ext, _T(".GMT") ) )
	{
		GModel* pModelObj =NULL;
		SAFE_NEW( pModelObj, GBoneObj );
		_ASSERT( pModelObj );
		m_pModelObj = pModelObj;
		pModelObj->m_ObjType = GMTFILE;
		return GMTFILE;
	}


	if (!_tcsicmp(Ext, _T(".TBS")))
	{
		GModel* pModelObj = NULL;
		SAFE_NEW(pModelObj, GGbsObj);
		_ASSERT(pModelObj);
		m_pModelObj = pModelObj;
		pModelObj->m_ObjType = TBSFILE;
		return TBSFILE;
	}
	if (!_tcsicmp(Ext, _T(".SKM")))
	{
		GModel* pModelObj = NULL;
		SAFE_NEW(pModelObj, GSkinObj);
		_ASSERT(pModelObj);
		m_pModelObj = pModelObj;
		pModelObj->m_ObjType = SKMFILE;
		return SKMFILE;
	}
	if (!_tcsicmp(Ext, _T(".MAT")))
	{
		GModel* pModelObj = NULL;
		SAFE_NEW(pModelObj, GBoneObj);
		_ASSERT(pModelObj);
		m_pModelObj = pModelObj;
		pModelObj->m_ObjType = MATFILE;
		return MATFILE;
	}


	return NULLFILE;
}

void GObjMgr::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	GTemplateMap< GModel >::TemplateMapItor itor;
	for (itor = TMap.begin(); itor != TMap.end(); itor++)
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
