#pragma once
#include "GBasisLib_0.h"
#include "GAseObj.h"
#include "GModelViewCamera.h"

class Sample : public GBASISLib_0
{
public:
	GAseObj		m_Object;
	shared_ptr<GCamera > m_pMainCamera;
	D3DXMATRIX m_matWorld;
public:
	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();
	HRESULT		CreateResource();
	HRESULT		DeleteResource();
	int			WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Sample(void);
	virtual ~Sample(void);
};
