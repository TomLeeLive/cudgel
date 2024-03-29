#pragma once

class Sample : public GBASISLib_0
{
public:
	shared_ptr<GCamera > m_pMainCamera;

	GAseModel	m_stModel;


	D3DXMATRIX				m_World;
	//D3DXMATRIX				m_View;
	//D3DXMATRIX				m_Projection;

	int			WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT		CreateResource();
	HRESULT		DeleteResource();

	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();
	Sample();
	virtual ~Sample();
};

