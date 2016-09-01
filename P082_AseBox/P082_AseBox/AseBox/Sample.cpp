#include "_stdafx.h"

//#define G_DEFINE_BOX
#define G_DEFINE_SHIP
//#define G_DEFINE_MULTICAMERAS


bool	Sample::Init() {
#ifdef G_DEFINE_BOX
	m_Parser.OpenStream(L"data/BOX.ASE");
#endif
#ifdef G_DEFINE_SHIP
	m_Parser.OpenStream(L"data/st02sc00.ASE");
#endif
#ifdef G_DEFINE_MULTICAMERAS
	m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif


	m_Parser.GetDataFromFile(&m_stModel);

	m_Parser.CloseStream();

	m_Parser.SetPnctData(&m_stModel);


	m_stModel.Init();


	// Initialize the world matrix
	D3DXMatrixIdentity(&m_World);



	// Initialize the view matrix
#ifdef G_DEFINE_BOX
	D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -7.0f);
#endif
#ifdef G_DEFINE_SHIP
	D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -700.0f);
#endif
#ifdef G_DEFINE_MULTICAMERAS
	D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -100.0f);
#endif

	D3DXVECTOR3 At = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_View, &Eye, &At, &Up);

	// Initialize the projection matrix
	D3DXMatrixPerspectiveFovLH(&m_Projection, XM_PIDIV2, m_iWindowWidth / (FLOAT)m_iWindowHeight, 0.01f, 10000.0f);
	
	return true;
};
bool	Sample::Frame() {

	// Update our time
	static float t = 0.0f;
	if (m_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	//
	// Animate the cube
	//
	D3DXMatrixRotationY(&m_World, t);

	m_stModel.Frame(&m_World,&m_View,&m_Projection);

	return true;
};
bool	Sample::Render() {


	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	g_pImmediateContext->ClearRenderTargetView(GetRenderTargetView(), ClearColor);


	m_stModel.Render();
	
	//
	// Present our back buffer to our front buffer
	//
	//m_pSwapChain->Present(0, 0);

	return true;
};
bool	Sample::Release() {



	m_stModel.Release();

	return true;
};

Sample::Sample()
{
}


Sample::~Sample()
{

}
GBASIS_RUN(L"Ase Box");