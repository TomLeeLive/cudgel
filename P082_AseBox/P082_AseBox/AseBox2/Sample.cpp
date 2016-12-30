#include "_stdafx.h"

//#define G_DEFINE_BOX
#define G_DEFINE_SHIP
//#define G_DEFINE_MULTICAMERAS
//#define G_DEFINE_ANI_TRANSLATE
//#define G_DEFINE_ANI_SCALE
//#define G_DEFINE_ANI_TURRET

//#define G_DEFINE_ANI_ROTATE

bool Sample::DrawDebug()
{

	if (I_Input.KeyCheck(DIK_N))
	{
		// 정점노말 표시
		m_LineDraw.SetMatrix(NULL, m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());

		D3DXMATRIX invWorld;
		D3DXMatrixIdentity(&invWorld);
		D3DXMatrixInverse(&invWorld, 0, m_pMainCamera->GetWorldMatrix());
		D3DXMatrixTranspose(&invWorld, &invWorld);
		D3DXVECTOR3 vStart, vEnd, vDir, vEye;
		vEye = *m_pMainCamera->GetLookVector();

		for (int i = 0; i < m_stModel.m_vGeomObj.size(); i++) {
			for (int j = 0; j < m_stModel.m_vGeomObj[i]->m_vObj.size(); j++) {
				for (int k = 0; k < m_stModel.m_vGeomObj[i]->m_vObj[j]->m_vPnctVertex.size(); k++) {
					D3DXVec3TransformCoord(&vStart, &m_stModel.m_vGeomObj[i]->m_vObj[j]->m_vPnctVertex[k].p, m_pMainCamera->GetWorldMatrix());
					D3DXVec3TransformNormal(&vDir, &m_stModel.m_vGeomObj[i]->m_vObj[j]->m_vPnctVertex[k].n, &invWorld);
					D3DXVec3Normalize(&vDir, &vDir);
					vEnd = vStart + vDir * 2.0f;
					float fDot = D3DXVec3Dot(&vEye, &vDir);
					if (fDot < 0)
					{
						vDir.x = vDir.x * 0.5f + 0.5f;
						vDir.y = vDir.y * 0.5f + 0.5f;
						vDir.z = vDir.z * 0.5f + 0.5f;
						m_LineDraw.Draw(m_pImmediateContext, vStart, vEnd, D3DXVECTOR4(vDir.x, vDir.y, vDir.z, 1.0f));
					}
				}
			}
		}

		/*
		for (UINT row = 0; row < m_HeightMap.m_iNumRows; row += 5)
		{
		for (UINT col = 0; col < m_HeightMap.m_iNumCols; col += 5)
		{
		D3DXVec3TransformCoord(&vStart, &m_HeightMap.m_VertexList[row*m_HeightMap.m_iNumCols + col].p, m_pMainCamera->GetWorldMatrix());
		D3DXVec3TransformNormal(&vDir, &m_HeightMap.m_VertexList[row*m_HeightMap.m_iNumCols + col].n, &invWorld);
		D3DXVec3Normalize(&vDir, &vDir);
		vEnd = vStart + vDir * 2.0f;
		float fDot = D3DXVec3Dot(&vEye, &vDir);
		if (fDot < 0)
		{
		vDir.x = vDir.x * 0.5f + 0.5f;
		vDir.y = vDir.y * 0.5f + 0.5f;
		vDir.z = vDir.z * 0.5f + 0.5f;
		m_LineDraw.Draw(m_pImmediateContext, vStart, vEnd, D3DXVECTOR4(vDir.x, vDir.y, vDir.z, 1.0f));
		}
		}
		}
		*/
	}

	if (!GBASISLib_0::DrawDebug()) return false;
	return true;
}

int Sample::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_pMainCamera != nullptr)
	{
		m_pMainCamera->WndProc(hWnd, msg, wParam, lParam);
	}
	return -1;
}
   
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


bool	Sample::Init() {
	if (FAILED(m_LineDraw.Create(GetDevice(), L"data/shader/line.hlsl")))
	{
		MessageBox(0, _T("m_LineDraw 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}

	m_cbLight.g_cAmbientMaterial = D3DXVECTOR4(0.3f, 0.3f, 0.3f, 1);
	m_cbLight.g_cDiffuseMaterial = D3DXVECTOR4(1, 1, 1, 1);
	m_cbLight.g_cAmbientLightColor = D3DXVECTOR4(1, 1, 1, 1);
	m_cbLight.g_cDiffuseLightColor = D3DXVECTOR4(1, 1, 1, 1);

	m_pConstantBufferLight.Attach(DX::CreateConstantBuffer(
		m_pd3dDevice, &m_cbLight, 1, sizeof(LIGHT_CONSTANT_BUFFER)));


#ifdef G_DEFINE_BOX
	m_stModel.Init(L"data/BOX.ASE", L"Tutorial04_2.fx");
	//m_Parser.OpenStream(L"data/BOX.ASE");
#endif
#ifdef G_DEFINE_SHIP
	m_stModel.Init(L"data/st02sc00.ASE", L"Tutorial04_2.fx");
	//m_Parser.OpenStream(L"data/st02sc00.ASE");
#endif
#ifdef G_DEFINE_MULTICAMERAS
	m_stModel.Init(L"data/MultiCameras.ASE", L"Tutorial04_2.fx");
	//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_TRANSLATE
		m_stModel.Init(L"data/boxtranslate.ASE", L"Tutorial04_2.fx");
		//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_SCALE
		m_stModel.Init(L"data/scaleanimationmodel.ASE", L"Tutorial04_2.fx");
		//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_ROTATE
		m_stModel.Init(L"data/rotanimation.ASE", L"Tutorial04_2.fx");
		//m_Parser.OpenStream(L"data/MultiCameras.ASE");
#endif
#ifdef G_DEFINE_ANI_TURRET
		m_stModel.Init(L"data/Turret_Deploy.ASE", L"Tutorial04_2.fx");
		//m_stModel.Init(L"data/mong.ASE", L"Tutorial04_2.fx");
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
bool	Sample::Frame() {



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


	//float t = m_Timer.GetElapsedTime() * D3DX_PI;
	D3DXMATRIX mLightWorld, mTranslate, mRotation;
	D3DXMatrixTranslation(&mTranslate, 100.0f, 100.0f, 0.0f);
	//D3DXMatrixRotationY(&mRotation, t*0.1f);
	D3DXMatrixIdentity(&mRotation);
	D3DXMatrixMultiply(&mLightWorld, &mTranslate, &mRotation);

	m_vLightVector.x = mLightWorld._41;
	m_vLightVector.y = mLightWorld._42;
	m_vLightVector.z = mLightWorld._43;

	D3DXVec3Normalize(&m_vLightVector, &m_vLightVector);
	m_vLightVector *= -1.0f;




	m_stModel.Frame();

	return true;
};
bool	Sample::Render() {

	DX::ApplyDSS(m_pImmediateContext, DX::GDxState::g_pDSSDepthEnable);
	DX::ApplyBS(m_pImmediateContext, DX::GDxState::g_pAlphaBlend);
	//
	// Clear the back buffer
	//
	//float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	//g_pImmediateContext->ClearRenderTargetView(GetRenderTargetView(), ClearColor);

	m_cbLight.g_vLightDir.x = m_vLightVector.x;
	m_cbLight.g_vLightDir.y = m_vLightVector.y;
	m_cbLight.g_vLightDir.z = m_vLightVector.z;
	m_cbLight.g_vLightDir.w = 1;
	D3DXMATRIX matInvWorld;
	D3DXMatrixInverse(&matInvWorld, NULL, &m_World);
	D3DXMatrixTranspose(&matInvWorld, &matInvWorld);
	D3DXMatrixTranspose(&m_cbLight.g_matInvWorld, &matInvWorld);

	m_pImmediateContext->UpdateSubresource(m_pConstantBufferLight.Get(), 0, NULL, &m_cbLight, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());
	m_pImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());

	m_stModel.Render(&m_World, m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());
	
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