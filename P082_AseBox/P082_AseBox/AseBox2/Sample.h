#pragma once

struct LIGHT_CONSTANT_BUFFER
{
	D3DXMATRIX			g_matInvWorld;
	// Material
	D3DXVECTOR4			g_cAmbientMaterial;
	D3DXVECTOR4			g_cDiffuseMaterial;
	D3DXVECTOR4			g_cSpecularMaterial;
	D3DXVECTOR4			g_cEmissionMaterial;
	// Light
	D3DXVECTOR4			g_cAmbientLightColor;
	D3DXVECTOR4			g_cDiffuseLightColor;
	D3DXVECTOR4			g_cSpecularLightColor;
	D3DXVECTOR4			g_vLightDir; // w = light damping(attenuation) : 감쇠
	D3DXVECTOR4			g_vLightPos; // w = light radius
	D3DXVECTOR4			g_vEyeDir;// w = light intensity : 강도
	D3DXVECTOR4			g_vEyePos;// w = light radius	
};

class Sample : public GBASISLib_0
{
public:
	bool		DrawDebug();
	GLineShape		m_LineDraw;

	//조명
	D3DXVECTOR3		m_vLightVector;
	LIGHT_CONSTANT_BUFFER m_cbLight;
	ComPtr<ID3D11Buffer>	m_pConstantBufferLight;

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

