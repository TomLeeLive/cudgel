#include "Sample.h"

bool	Sample::Init()
{
	m_pSRV[0].Attach(DX::CreateShaderResourceView(m_pd3dDevice, 
		L"../../../data/MSH1024_Mask.DDS"));
	m_pSRV[1].Attach(DX::CreateShaderResourceView(m_pd3dDevice,
		L"../../../data/DetailMap_Red.dds"));
	m_pSRV[2].Attach(DX::CreateShaderResourceView(m_pd3dDevice,
		L"../../../data/DetailMap_Green.dds"));
	m_pSRV[3].Attach(DX::CreateShaderResourceView(m_pd3dDevice,
		L"../../../data/DetailMap_Blue.dds"));
	m_pSRV[4].Attach(DX::CreateShaderResourceView(m_pd3dDevice,
		L"../../../data/DetailMap_Alpha.dds"));

	m_pPS[0].Attach(DX::LoadPixelShaderFile(m_pd3dDevice,
		L"BLEND.HLSL", "blendps"));
	m_pPS[1].Attach(DX::LoadPixelShaderFile(m_pd3dDevice,
		L"BLEND.HLSL", "PS_Color"));

	D3D11_BLEND_DESC bsd;
	ZeroMemory(&bsd, sizeof(D3D11_BLEND_DESC));
	bsd.RenderTarget[0].BlendEnable = TRUE;
	// rgb에 대한 대상과 소스에 대한 혼합 방법 지정
	bsd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	// R =  DEST.R * ( 1- SRCAPHA ) + SRC.R * SRCALPHA;
	// G =  DEST.G * ( 1- SRCAPHA ) + SRC.G * SRCALPHA;
	// B =  DEST.B * ( 1- SRCAPHA ) + SRC.B * SRCALPHA;
	// A
	bsd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bsd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bsd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	// 소스 + 대상 작업완료 이후에 RGBA 결과를 백버퍼에 출력
	// r g b a
	// 1 0 1 1

	bsd.RenderTarget[0].RenderTargetWriteMask = 
		D3D11_COLOR_WRITE_ENABLE_RED |
		D3D11_COLOR_WRITE_ENABLE_GREEN |
		D3D11_COLOR_WRITE_ENABLE_BLUE |
		D3D11_COLOR_WRITE_ENABLE_ALPHA;

	m_pd3dDevice->CreateBlendState(&bsd, m_pAlphaBlend.GetAddressOf());

	m_pShape[0] = nullptr;
	m_pShape[0] = make_shared<TBoxShape>();
	m_pShape[1] = make_shared<TPlaneShape>();
	m_pShape[2] = make_shared<TPlaneShape>();

	if (!m_pShape[0]->Create(g_pd3dDevice,
		L"blend.hlsl", L"../../../data/hud.dds"))
	{
		return false;
	}
	if (!m_pShape[1]->Create(g_pd3dDevice,
		L"blend.hlsl", L"../../../data/Front.bmp"))
	{
		return false;
	}
	if (!m_pShape[2]->Create(g_pd3dDevice,
		L"blend.hlsl",
		L"../../../data/Back.bmp"))
	{
		return false;
	}
	for (int iObj = 0; iObj < MAX_CNT; iObj++)
	{
		D3DXMatrixIdentity(&m_matWorld[iObj]);
	}
	D3DXMatrixScaling(&m_matWorld[0], 30, 30, 30);
	D3DXMatrixScaling(&m_matWorld[1], 3, 3, 3);
	D3DXMatrixScaling(&m_matWorld[2], 3, 3, 3);
	m_matWorld[1]._43 = 0.1f;
	m_matWorld[2]._43 = -1.0f;

	if (!m_Line.Create(g_pd3dDevice,
		L"../../../data/shader/Line.hlsl", 0))
	{
		return false;
	}

	m_Camera.Create(m_pd3dDevice);

	return true;
}
bool	Sample::Frame() {

	D3DXMATRIX matRotation, matScale;
	D3DXMatrixScaling(&matScale, 10, 10, 10);
	D3DXMatrixRotationX(&matRotation,
		D3DXToRadian(90.0f));
	m_matWorld[1] = matScale * matRotation;

	/*for (int iObj = 1; iObj < MAX_CNT; iObj++)
	{
	if (iObj == 2)
	{
	m_matWorld[iObj]._41 = iObj *
	10.0f * cosf(m_Timer.m_fAccumulation);

	D3DXMatrixTranslation(
	&matTrans,
	-iObj * 10.0f * cosf(m_Timer.m_fAccumulation),
	0,
	0);
	}

	D3DXMatrixRotationY(&matRotation,
	m_Timer.m_fAccumulation);

	m_matWorld[iObj] = matTrans * matRotation;
	}*/
	if (I_Input.m_DIMouseState.rgbButtons[0])
	{
		m_vDir.y += m_fSpeed * g_fSecPerFrame * I_Input.m_DIMouseState.lX;
		m_vDir.x += m_fSpeed * g_fSecPerFrame * I_Input.m_DIMouseState.lY;
	}
	if (I_Input.KeyCheck(DIK_SPACE))
	{
		m_fSpeed += g_fSecPerFrame * 20.0f;
		if (m_fSpeed >= 50.0f) m_fSpeed = 50.0f;
	}
	else
	{
		m_fSpeed -= g_fSecPerFrame * 10.0f;
		if (m_fSpeed <= 5.0f) m_fSpeed = 5.0f;
	}
	if (I_Input.KeyCheck(DIK_W))
	{
		m_Camera.MoveLook(g_fSecPerFrame* m_fSpeed);
	}
	if (I_Input.KeyCheck(DIK_S))
	{
		m_Camera.MoveLook(-g_fSecPerFrame* m_fSpeed);
	}
	m_vDir.w = 0.0f;
	if (I_Input.m_DIMouseState.lZ != 0)
	{
		m_vDir.w = m_fSpeed * g_fSecPerFrame * I_Input.m_DIMouseState.lZ;
	}


	if (I_Input.KeyCheck(DIK_F4))
	{
		m_bCameraType = !m_bCameraType;
	}
	if (m_bCameraType)
	{
		m_Camera.Update(m_vDir);
	}
	else
	{
		m_Camera.Frame(m_vDir);
	}
	return true;
}
bool	Sample::Render()
{
	g_pImmediateContext->OMSetBlendState(m_pAlphaBlend.Get(), 0, -1);
	g_pImmediateContext->PSSetShaderResources(1, 5, m_pSRV[0].GetAddressOf());

	float t = cosf(m_Timer.m_fAccumulation) *0.5f + 0.5f;
	for (int iObj = 0; iObj < 2; iObj++)
	{
		m_pShape[iObj]->m_cbData.Color = D3DXVECTOR4(
			1,1,1, t);
		g_pImmediateContext->PSSetConstantBuffers(0, 1,
			m_pShape[iObj]->m_Object.g_pConstantBuffer.GetAddressOf());

		m_pShape[iObj]->SetMatrix(
			&m_matWorld[iObj],
			&m_Camera.m_matView,
			&m_Camera.m_matProj);

		m_pShape[iObj]->PreRender(g_pImmediateContext);	
		if( iObj == 0 )
			g_pImmediateContext->PSSetShader(m_pPS[1].Get(), NULL, 0);
		else
			g_pImmediateContext->PSSetShader(m_pPS[0].Get(), NULL, 0);
		m_pShape[iObj]->PostRender(g_pImmediateContext);

		//m_pShape[iObj]->Render(g_pImmediateContext);
	}

	/*D3DXMATRIX matScale, matRotation, matWorld;
	D3DXMatrixScaling(&matScale, 5, 5, 5);
	matWorld = matScale * m_matWorld[1];
	m_pShape[0]->SetMatrix(
		&matWorld,
		&m_Camera.m_matView,
		&m_Camera.m_matProj);
	m_pShape[0]->Render(g_pImmediateContext);

	matWorld = matScale * m_matWorld[2];
	m_pShape[0]->SetMatrix(
		&matWorld,
		&m_Camera.m_matView,
		&m_Camera.m_matProj);
	m_pShape[0]->Render(g_pImmediateContext);*/

	return true;
}
bool	Sample::Release() {
	return true;
}

Sample::Sample()
{
	m_vDir = D3DXVECTOR4(0, 0, 0, 10);
	m_fSpeed = 5.0f;
	m_bCameraType = true;
}


Sample::~Sample()
{
}
TCORE_RUN(_T("Sample Lib"), 800, 600)