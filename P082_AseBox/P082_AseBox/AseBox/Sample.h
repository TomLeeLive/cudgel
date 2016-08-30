#pragma once








class Sample : public GBASISLib_0
{
public:
	GAseModel	m_stModel;
	GAseParser	m_Parser;







	D3DXMATRIX				m_World;
	D3DXMATRIX				m_View;
	D3DXMATRIX				m_Projection;







	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();
	Sample();
	virtual ~Sample();
};

