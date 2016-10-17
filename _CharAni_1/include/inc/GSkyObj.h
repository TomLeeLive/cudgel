#pragma once
#include "GBoxObj.h"
class GSkyObj : public GBoxObj
{
public:
	void SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	GSkyObj(void);
	virtual ~GSkyObj(void);
};
