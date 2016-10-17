#pragma once
#include "GTemplate.h"
#include "GAseObj.h"
#include "GGbsObj.h"
#include "GSkinObj.h"
#include "GBoneObj.h"

class GObjMgr : public GTemplateMap< GModel >, public GSingleton<GObjMgr>
{
private:
	friend class GSingleton<GObjMgr>;
	GModel*		m_pModelObj;
public:
	int			Load(ID3D11Device* pd3dDevice,
		const TCHAR* strFileName,
		const TCHAR* strShaderName, bool bThread = false);
	
	FILETYPE	GetFileTypeID(const TCHAR* pszFileName);
	void		SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
private:
	GObjMgr(void);
	virtual ~GObjMgr(void);
};

static GAseObj* IsAseObjType(GModel* pModel)
{
	const std::type_info& r1 = typeid(*pModel);
	const std::type_info& r2 = typeid(GAseObj);
	GAseObj* pObj = nullptr;
	if (&r1 == &r2)
	{
		pObj = dynamic_cast<GAseObj*>(pModel);
	}	
	return pObj;
}
static GGbsObj* IsTbsObjType(GModel* pModel)
{
	const std::type_info& r1 = typeid(*pModel);
	const std::type_info& r2 = typeid(GGbsObj);
	GGbsObj* pObj = nullptr;
	if (&r1 == &r2)
	{
		pObj = dynamic_cast<GGbsObj*>(pModel);
	}
	return pObj;
}
template<typename T, typename F>
auto GObjMgr::execute(T* value, F func) -> decltype(func(value)) 
{
	return func(value);
}
#define I_ObjMgr GObjMgr::GetInstance()