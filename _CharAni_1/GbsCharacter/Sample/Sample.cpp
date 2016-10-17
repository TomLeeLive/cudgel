#include "Sample.h"
#include "GTimer.h"

bool Sample::Init()
{	
	if( !m_Object.Init() )	return false;
	CStopwatch stopwatch;
	if (!m_Object.Load(GetDevice(),
		_T("data/BOX.ase"),
		L"data/shader/box.hlsl"))
	{
		return false;
	}		
	stopwatch.Output(L"Init()");

	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<GCamera>();
	m_pMainCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, -5.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	float fAspectRatio = m_iWindowWidth / (FLOAT)m_iWindowHeight;
	m_pMainCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f);
	m_pMainCamera->SetWindow(m_iWindowWidth, m_iWindowHeight);

	return true;
}
bool Sample::Render()
{
	m_Object.SetMatrix(NULL, m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());
	return m_Object.Render(m_pImmediateContext);
}
bool Sample::Release()
{
	return m_Object.Release();
}

bool Sample::Frame()
{
	m_pMainCamera->Frame();// (g_fSecPerFrame);
	m_matWorld = *m_pMainCamera->GetWorldMatrix();
	return m_Object.Frame();
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT Sample::CreateResource()
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
HRESULT Sample::DeleteResource()
{
	HRESULT hr = S_OK;
	if (m_pImmediateContext) m_pImmediateContext->ClearState();
	return S_OK;
}
Sample::Sample(void)
{
}

Sample::~Sample(void)
{
}
int Sample::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_pMainCamera != nullptr)
	{
		m_pMainCamera->WndProc(hWnd, msg, wParam, lParam);
	}
	return -1;
}
GBASIS_RUN(L"MultiAnimation");





