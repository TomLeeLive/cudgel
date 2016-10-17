#pragma once
#include "GTemplate.h"
#include "GAseObj.h"

class GObjMgr : public GTemplateMap< GModel >, public GSingleton<GObjMgr>
{
private:
	friend class GSingleton<GObjMgr>;
	GModel*		m_pModelObj;
public:
	int			Load(ID3D11Device* pd3dDevice,
		const TCHAR* strFileName,
		const TCHAR* strShaderName);
	FILETYPE	GetFileTypeID(const TCHAR* pszFileName);
	void		SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
private:
	GObjMgr(void);
	virtual ~GObjMgr(void);
};
#define I_ObjMgr GObjMgr::GetInstance()