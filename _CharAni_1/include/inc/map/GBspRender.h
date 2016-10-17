#pragma once
#include "GSpaceDivision.h"

class GBspRender : public GModel
{
public:
	void		RenderLeafBoungingBox(ID3D11DeviceContext*		pContext, GCamera* pCamera, GNode* pNode );
	void		RenderNode(ID3D11DeviceContext*		pContext, GNode* pNode );
	void		RecursionRender(ID3D11DeviceContext*		pContext, GNode* pNode );
public:
	GBspRender(void);
	virtual ~GBspRender(void);
};
