#include "_stdafx.h"

//#define G_DEFINE_BOX
//#define G_DEFINE_SHIP
//#define G_DEFINE_MULTICAMERAS
//#define G_DEFINE_ANI_TRANSLATE
//#define G_DEFINE_ANI_SCALE
#define G_DEFINE_ANI_TURRET

//#define G_DEFINE_ANI_ROTATE

int GbsViewer::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_pMainCamera != nullptr)
	{
		m_pMainCamera->WndProc(hWnd, msg, wParam, lParam);
	}
	return -1;
}
   
HRESULT GbsViewer::CreateResource()
{
	HRESULT hr;
	if (m_pMainCamera)
	{
		float fAspectRatio = m_SwapChainDesc.BufferDesc.Width /
			(float)m_SwapChainDesc.BufferDesc.Height;
		m_pMainCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f);
	}
	return S_OK;
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT GbsViewer::DeleteResource()
{
	HRESULT hr = S_OK;
	if (m_pImmediateContext) m_pImmediateContext->ClearState();
	return S_OK;
}


bool	GbsViewer::Init() {

#ifdef G_DEFINE_BOX
	m_stModel.Init(L"data/BOX.ASE", L"Tutorial04.fx");
	//m_Parser.OpenStream(L"data/BOX.ASE");
#endif
#ifdef G_DEFINE_SHIP
	m_stModel.Init(L"data/st02sc00.ASE", L"Tutorial04.fx");
	//m_Parser.OpenStream(L"data/st02sc00.ASE");
#endif
#ifdef G_DEFINE_MULTICAMERAS
	m_stModel.Init(L"data/MultiCameras.ASE", L"Tutorial04.fx");
	//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_TRANSLATE
		m_stModel.Init(L"data/boxtranslate.ASE", L"Tutorial04.fx");
		//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_SCALE
		m_stModel.Init(L"data/scaleanimationmodel.ASE", L"Tutorial04.fx");
		//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_ROTATE
		m_stModel.Init(L"data/rotanimation.ASE", L"Tutorial04.fx");
		//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_TURRET
		m_stModel.Init(L"data/Turret_Deploy.ASE", L"Tutorial04.fx");
		//m_stModel.Init(L"data/mong.ASE", L"Tutorial04.fx");
		//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
	


	// Initialize the world matrix
	D3DXMatrixIdentity(&m_World);


	//     // Initialize the view matrix
	//#ifdef G_DEFINE_BOX
	//     D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -7.0f);
	//#endif
	//#ifdef G_DEFINE_SHIP
	//     D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -700.0f);
	//#endif
	//#ifdef G_DEFINE_MULTICAMERAS
	//     D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -100.0f);
	//#endif
	//
	//     D3DXVECTOR3 At = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//     D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//     D3DXMatrixLookAtLH(&m_View, &Eye, &At, &Up);

	//// Initialize the projection matrix
	//D3DXMatrixPerspectiveFovLH(&m_Projection, XM_PIDIV2, m_iWindowWidth / (FLOAT)m_iWindowHeight, 0.01f, 10000.0f);



	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<GCamera>();
	m_pMainCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 100.0f, -100.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	float fAspectRatio = m_iWindowWidth / (FLOAT)m_iWindowHeight;
	m_pMainCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f);
	m_pMainCamera->SetWindow(m_iWindowWidth, m_iWindowHeight);

	return true;
};
bool	GbsViewer::Frame() {



#ifdef TESTTEST
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
#endif


	m_pMainCamera->Frame();// (g_fSecPerFrame);
	m_World = *m_pMainCamera->GetWorldMatrix();

	m_stModel.Frame();

	return true;
};
bool	GbsViewer::Render() {


	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	g_pImmediateContext->ClearRenderTargetView(GetRenderTargetView(), ClearColor);


	m_stModel.Render(&m_World, m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());
	
	//
	// Present our back buffer to our front buffer
	//
	//m_pSwapChain->Present(0, 0);

	return true;
};
bool	GbsViewer::Release() {



	m_stModel.Release();

	return true;
};

GbsViewer::GbsViewer()
{
}


GbsViewer::~GbsViewer()
{

}
GBASIS_RUN(L"Ase Box");