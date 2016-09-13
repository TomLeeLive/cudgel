#include "_stdafx.h"



bool		GAseModel::Init(TCHAR* strFileName, TCHAR* strShaderName) {

	HRESULT hr = S_OK;

	//파싱, 필요한 처리 시작.
	CStopwatch stopwatch;


	//I_AseParser
	I_AseParser.InitAseModel(strFileName, this);

	//필요한 처리 끝.
	stopwatch.Output(L"Init()");


	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, m_pSamplerLinear.GetAddressOf());
	if (FAILED(hr))
		return hr;




	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(strShaderName, "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), m_pVertexLayout.GetAddressOf());
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(m_pVertexLayout.Get());

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(strShaderName, "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf());
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;


	

	if (m_vGeomObj.size() == 1)
		SingleModelInit();
	else
		MultiModelInit();

	


	return true;
};
bool GAseModel::MultiModelInit() {
	HRESULT hr = S_OK;

	//Create Constant Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;


	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, m_pConstantBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;


	for (int j = 0; j < m_vMaterial.size(); j++) {
		for (int i = 0; i < m_vMaterial[j]->m_vSubMaterial.size(); i++) {

			if (_tcslen(m_vMaterial[j]->m_vSubMaterial[i]->m_szMapDiffuse) == 0)
				continue;

			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, m_vMaterial[j]->m_vSubMaterial[i]->m_szMapDiffuse, NULL, NULL, m_vMaterial[j]->m_vSubMaterial[i]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))
				return hr;
		}
	}




	for (int i = 0; i < m_vGeomObj.size()/*m_vMaterial[0]->m_vSubMaterial.size()*/; i++) {

		for (int j = 0; j < m_vGeomObj[i]->m_vObj.size(); j++) {
		
			if (m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size() == 0)
				continue;


			//D3D11_BUFFER_DESC bd;
			//ZeroMemory(&bd, sizeof(bd));
			//bd.Usage = D3D11_USAGE_DEFAULT;
			//bd.ByteWidth = sizeof(ConstantBuffer);
			//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			//bd.CPUAccessFlags = 0;

			//hr = g_pd3dDevice->CreateBuffer(&bd, NULL, m_vObj[i]->m_pConstantBuffer.GetAddressOf());
			//if (FAILED(hr))
			//	return hr;






			//}


			//for (int i = 1; i < m_vMaterial[0]->m_iSubMaterial + 1; i++) {

			// Create vertex buffer
			PNCT_VERTEX* vertices;
			vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size());

			for (int k = 0; k < m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size(); ++k) {

				vertices[k] = m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex[k];
			}

			//D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(PNCT_VERTEX) * m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size();//m_iPosCount;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = vertices;
			hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vGeomObj[i].get()->m_vObj[j]->m_pVertexBuffer.GetAddressOf());
			if (FAILED(hr))
				return hr;

			// Set vertex buffer
			UINT stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;
			g_pImmediateContext->IASetVertexBuffers(0, 1, m_vGeomObj[i].get()->m_vObj[j]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);


			// Create index buffer
			WORD* indices;
			indices = (WORD *)malloc(sizeof(WORD) * m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size());

			for (int k = 0; k < m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size(); k++) {
				indices[k] = k;
			}


			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(WORD) * m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size();        // 36 vertices needed for 12 triangles in a triangle list
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = indices;
			hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vGeomObj[i].get()->m_vObj[j]->m_pIndexBuffer.GetAddressOf());
			if (FAILED(hr))
				return hr;

			// Set index buffer
			g_pImmediateContext->IASetIndexBuffer(m_vGeomObj[i].get()->m_vObj[j]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

			// Set primitive topology
			g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


			delete[] indices;
			delete[] vertices;




		}
	}
		

	
	return hr;

}

bool GAseModel::SingleModelInit() {
	HRESULT hr = S_OK;

	//Create Constant Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;


	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, m_pConstantBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	if (m_vMaterial[0]->m_vSubMaterial.size() == 0) {



		// Load the Texture
		hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, m_vMaterial[0]->m_szMapDiffuse, NULL, NULL, m_vMaterial[0]->m_pTextureRV.GetAddressOf(), NULL);
		if (FAILED(hr))
			return hr;

		// Create vertex buffer
		PNCT_VERTEX* vertices;
		vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex.size());

		for (int j = 0; j < m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex.size(); ++j) {

			vertices[j] = m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex[j];

		}

		//D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(PNCT_VERTEX) * m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex.size();//m_iPosCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;
		hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vGeomObj[0].get()->m_vObj[0]->m_pVertexBuffer.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Set vertex buffer
		UINT stride = sizeof(PNCT_VERTEX);
		UINT offset = 0;
		g_pImmediateContext->IASetVertexBuffers(0, 1, m_vGeomObj[0].get()->m_vObj[0]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);


		// Create index buffer
		WORD* indices;
		indices = (WORD *)malloc(sizeof(WORD) * m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex.size());

		for (int j = 0; j < m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex.size(); j++) {
			indices[j] = j;
		}


		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex.size();        // 36 vertices needed for 12 triangles in a triangle list
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;
		hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vGeomObj[0].get()->m_vObj[0]->m_pIndexBuffer.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Set index buffer
		g_pImmediateContext->IASetIndexBuffer(m_vGeomObj[0].get()->m_vObj[0]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		// Set primitive topology
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);








		delete[] indices;
		delete[] vertices;



	}
	else {


		for (int i = 0; i < m_vMaterial[0]->m_vSubMaterial.size(); i++) {
			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, m_vMaterial[0]->m_vSubMaterial[i]->m_szMapDiffuse, NULL, NULL, m_vMaterial[0]->m_vSubMaterial[i]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))
				return hr;
		}


		for (int i = 0; i < m_vMaterial[0]->m_vSubMaterial.size(); i++) {

			//D3D11_BUFFER_DESC bd;
			//ZeroMemory(&bd, sizeof(bd));
			//bd.Usage = D3D11_USAGE_DEFAULT;
			//bd.ByteWidth = sizeof(ConstantBuffer);
			//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			//bd.CPUAccessFlags = 0;

			//hr = g_pd3dDevice->CreateBuffer(&bd, NULL, m_vObj[i]->m_pConstantBuffer.GetAddressOf());
			//if (FAILED(hr))
			//	return hr;






			//}


			//for (int i = 1; i < m_vMaterial[0]->m_iSubMaterial + 1; i++) {

			// Create vertex buffer
			PNCT_VERTEX* vertices;
			vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size());

			for (int j = 0; j < m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size(); ++j) {

				vertices[j] = m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex[j];

			}

			//D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(PNCT_VERTEX) * m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size();//m_iPosCount;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = vertices;
			hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vGeomObj[0].get()->m_vObj[i]->m_pVertexBuffer.GetAddressOf());
			if (FAILED(hr))
				return hr;

			// Set vertex buffer
			UINT stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;
			g_pImmediateContext->IASetVertexBuffers(0, 1, m_vGeomObj[0].get()->m_vObj[i]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);


			// Create index buffer
			WORD* indices;
			indices = (WORD *)malloc(sizeof(WORD) * m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size());

			for (int j = 0; j < m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size(); j++) {
				indices[j] = j;
			}


			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(WORD) * m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size();        // 36 vertices needed for 12 triangles in a triangle list
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = indices;
			hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vGeomObj[0].get()->m_vObj[i]->m_pIndexBuffer.GetAddressOf());
			if (FAILED(hr))
				return hr;

			// Set index buffer
			g_pImmediateContext->IASetIndexBuffer(m_vGeomObj[0].get()->m_vObj[i]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

			// Set primitive topology
			g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


			delete[] indices;
			delete[] vertices;

		}


	}
}
void		GAseModel::GetAnimationTrack(float fCurrentTick, GAnimTrack** pStartTrack, GAnimTrack** pEndTrack, ANITRACK_TYPE nTrackType, int iGeomNum) {

	switch (nTrackType) {
		case ANITRACK_TYPE_POS:
		{
			for (int j = 0; j< m_vGeomObj[iGeomNum].get()->m_vPosTrack.size(); j++) {

				//if(m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get())

				if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get()->iTick
					&& m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get()->pNext == NULL)
				{
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get();
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get();
					break;
				}

				else if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get()->iTick
					&& m_fTickFrame < m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get()->pNext->iTick)
				{
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get();
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vPosTrack[j].get()->pNext;
					break;
				}
				else if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vPosTrack[ m_vGeomObj[iGeomNum].get()->m_vPosTrack.size() - 1].get()->iTick)
				{
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vPosTrack[m_vGeomObj[iGeomNum].get()->m_vPosTrack.size() - 1].get();
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vPosTrack[m_vGeomObj[iGeomNum].get()->m_vPosTrack.size() - 1].get();
				}
			}
		}
		break;
		case ANITRACK_TYPE_ROT:
		{
			for (int j = 0; j< m_vGeomObj[iGeomNum].get()->m_vRotTrack.size(); j++) {
				if (m_fTickFrame < m_vGeomObj[iGeomNum].get()->m_vRotTrack[0].get()->iTick) {
					*pStartTrack = NULL;
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vRotTrack[0].get();
					break;
				}
				else if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vRotTrack[  m_vGeomObj[iGeomNum].get()->m_vRotTrack.size() - 1  ].get()->iTick) {
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vRotTrack[  m_vGeomObj[iGeomNum].get()->m_vRotTrack.size() - 1 ].get();
					*pEndTrack = NULL;
					break;
				}

				if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vRotTrack[j].get()->iTick
					&& m_fTickFrame < m_vGeomObj[iGeomNum].get()->m_vRotTrack[j].get()->pNext->iTick)
				{
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vRotTrack[j].get();
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vRotTrack[j].get()->pNext;
					break;
				}

				/*
				if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vRotTrack[m_vGeomObj[iGeomNum].get()->m_vRotTrack.size() - 1].get()->iTick)
				{
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vRotTrack[m_vGeomObj[iGeomNum].get()->m_vRotTrack.size() - 1].get();
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vRotTrack[m_vGeomObj[iGeomNum].get()->m_vRotTrack.size() - 1].get();
				}
				*/
			}
		}
		break;
		case ANITRACK_TYPE_SCL:
		{
			for (int j = 0; j< m_vGeomObj[iGeomNum].get()->m_vSclTrack.size(); j++) {

				if (m_fTickFrame < m_vGeomObj[iGeomNum].get()->m_vSclTrack[0].get()->iTick) {
					*pStartTrack = NULL;
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vSclTrack[0].get();
					break;
				}
				else if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vSclTrack[m_vGeomObj[iGeomNum].get()->m_vSclTrack.size() - 1 ].get()->iTick) {
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vSclTrack[m_vGeomObj[iGeomNum].get()->m_vSclTrack.size() - 1 ].get()->pNext;
					*pEndTrack = NULL;
					break;
				}

				if (m_fTickFrame >= m_vGeomObj[iGeomNum].get()->m_vSclTrack[j].get()->iTick
					&& m_fTickFrame < m_vGeomObj[iGeomNum].get()->m_vSclTrack[j].get()->pNext->iTick)
				{
					*pStartTrack = m_vGeomObj[iGeomNum].get()->m_vSclTrack[j].get();
					*pEndTrack = m_vGeomObj[iGeomNum].get()->m_vSclTrack[j].get()->pNext;
					break;
				}

			}
		}
		break;
	}

}

void		GAseModel::MultiAniFrame(){

	D3DXMATRIX matWldTrans;
	D3DXMATRIX matWldRotate;
	D3DXMATRIX matWldScale;
	D3DXMatrixIdentity(&matWldTrans);
	D3DXMatrixIdentity(&matWldRotate);
	D3DXMatrixIdentity(&matWldScale);
	

	D3DXQUATERNION qR, qS;

	D3DXMATRIX matCalc;
	D3DXMatrixIdentity(&matCalc);


	//m_fTickFrame = 6400.0f;
	m_fTickFrame += g_fSecPerFrame * m_fFrameSpeed *m_fTickPerFrame;
	if (m_fTickFrame >= m_fLastFrame * m_fTickPerFrame /*마지막 프레임 틱수*/)
	{
		m_fTickFrame = 0.0f;
	}

	//m_fTickFrame += 1000.0f;
	//if (m_fTickFrame >= 8000.0f /*마지막 프레임 틱수*/)
	//{
	//	m_fTickFrame = 0.0f;
	//}

	for (int i = 0; i < m_vGeomObj.size() ; i++) {

		if (m_vGeomObj[i]->m_bUsed == false)
			continue;

		D3DXMatrixIdentity(&m_vGeomObj[i]->m_matCalculation);

		matWldRotate = m_vGeomObj[i]->m_matWldRotate;
		matWldTrans = m_vGeomObj[i]->m_matWldTrans;
		matWldScale = m_vGeomObj[i]->m_matWldScale;

		D3DXQuaternionRotationMatrix(&qR, &matWldRotate);
		D3DXQuaternionRotationMatrix(&qS, &matWldScale);
		//for (int j = 0; j < m_vGeomObj[i]->m_vObj.size(); j++) {

			//if (m_vGeomObj[i].get()->m_bHasAniTrack) {






		//Translation
		if (m_vGeomObj[i].get()->m_vPosTrack.size() != 0) {

			GAnimTrack* pStartTrack = NULL;
			GAnimTrack* pEndTrack = NULL;

			//현재 Tick이 어디인지 찾자.
			GetAnimationTrack(m_fTickFrame, &pStartTrack, &pEndTrack, ANITRACK_TYPE_POS, i);

			//애니메이션 보간.
			D3DXVECTOR3 vResultVector;
			D3DXVECTOR3 vP1 = pStartTrack->vecVector;
			D3DXVECTOR3 vP2 = pEndTrack->vecVector;

			float fTValue = (m_fTickFrame - pStartTrack->iTick) / (pEndTrack->iTick - pStartTrack->iTick);

			if(pStartTrack != pEndTrack){
				D3DXVec3Lerp(&vResultVector, &vP1, &vP2, fTValue);

				//T행렬 값 대입
				matWldTrans._41 = vResultVector.x;
				matWldTrans._42 = vResultVector.y;
				matWldTrans._43 = vResultVector.z;
			}
			else {
				//T행렬 값 대입
				matWldTrans._41 = pStartTrack->vecVector.x;
				matWldTrans._42 = pStartTrack->vecVector.y;
				matWldTrans._43 = pStartTrack->vecVector.z;
			}
		}

		//Rotation
		if (m_vGeomObj[i].get()->m_vRotTrack.size() != 0) {
			GAnimTrack* pStartTrack = NULL;
			GAnimTrack* pEndTrack = NULL;
			//D3DXQUATERNION qR;

			//현재 Tick이 어디인지 찾자.
			GetAnimationTrack(m_fTickFrame, &pStartTrack, &pEndTrack, ANITRACK_TYPE_ROT, i);


			//사원수간의 보간..
			if (pStartTrack == NULL && pEndTrack == NULL) {
				qR = m_vGeomObj[i].get()->m_vRotTrack[0]->qRotate;// = m_vGeomObj[i].get()->m_qRotation;
			}
			else if (pStartTrack == NULL) {
				qR;// = m_vGeomObj[i].get()->m_qRotation;
				float fTValue = (m_fTickFrame - 0) / (pEndTrack->iTick - 0);
				D3DXQuaternionSlerp(&qR, &qR, &pEndTrack->qRotate, fTValue);
			}
			else if (pEndTrack == NULL) {
				qR = m_vGeomObj[i].get()->m_vRotTrack[m_vGeomObj[i].get()->m_vRotTrack.size() - 1].get()->qRotate;
				float fTValue = ((m_fTickFrame - pStartTrack->iTick) / (m_fFrameSpeed*m_fTickPerFrame));
				D3DXQuaternionSlerp(&qR, &qR, &qR, fTValue);
			}
			else {
				qR = pStartTrack->qRotate;
				float fTValue = (m_fTickFrame - pStartTrack->iTick) / (pEndTrack->iTick - pStartTrack->iTick);
				D3DXQuaternionSlerp(&qR, &qR, &pEndTrack->qRotate, fTValue);
			}

			//사원수에서 행렬로 변환.
			D3DXMatrixRotationQuaternion(&matWldRotate, &qR);// 사원수에서 행렬로 변환
		}

		//Scale
		if (m_vGeomObj[i].get()->m_vSclTrack.size() != 0) {

			GAnimTrack* pStartTrack = NULL;
			GAnimTrack* pEndTrack = NULL;

			D3DXMATRIX matScaleRot, matInvScaleRot;
			D3DXMatrixIdentity(&matScaleRot);
			D3DXMatrixIdentity(&matInvScaleRot);

			//D3DXQUATERNION qS;
			float fStartTick = 0.0f, fEndTick = 0.0f;
			D3DXVECTOR3 vScale(m_vGeomObj[i].get()->m_matWldScale._11, m_vGeomObj[i].get()->m_matWldScale._22, m_vGeomObj[i].get()->m_matWldScale._33);

			//현재 Tick이 어디인지 찾자.
			GetAnimationTrack(m_fTickFrame, &pStartTrack, &pEndTrack, ANITRACK_TYPE_SCL, i);

			//신축트랙 보간
			if (pStartTrack == NULL) {
				//vScale = m_vGeomObj[i].get()->m_vecTM_SCALE;

				//D3DXQuaternionRotationAxis(&qS, &m_vGeomObj[i].get()->m_vecTM_SCALE_AXIS, m_vGeomObj[i].get()->m_fTM_SCALEAXISANG);

				fStartTick = 0.0f;
				fEndTick = pEndTrack->iTick;
			}
			else if (pEndTrack == NULL) {
				vScale = pStartTrack->vecVector;
				qS = pStartTrack->qRotate;

				fStartTick = pStartTrack->iTick;

				fEndTick = pStartTrack->iTick + (m_fFrameSpeed*m_fTickPerFrame);
			}
			else {
				vScale = pStartTrack->vecVector;
				qS = pStartTrack->qRotate;

				fStartTick = pStartTrack->iTick;
				fEndTick = pEndTrack->iTick;


			}
			float fTValue = (m_fTickFrame - fStartTick) / (fEndTick - fStartTick);

			D3DXVec3Lerp(&vScale, &vScale, &pEndTrack->vecVector, fTValue);
			D3DXQuaternionSlerp(&qS, &qS, &pEndTrack->qRotate, fTValue);

			//사원수 -> 행렬로 변환등...
			D3DXMatrixScaling(&matWldScale, vScale.x, vScale.y, vScale.z);
			D3DXMatrixRotationQuaternion(&matScaleRot, &qS);
			D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot);

			matWldScale = matInvScaleRot * matWldScale * matScaleRot;



		}

		if (m_vGeomObj[i].get()->m_pParentObj != NULL) {

			m_vGeomObj[i].get()->m_matCalculation  = matCalc = matWldScale * matWldRotate * matWldTrans
				* m_vGeomObj[i].get()->m_pParentObj->m_matCalculation;
			int iTest = 0;

			// 인버스 매트릭스 확인 코드.
			D3DXVECTOR3 v0, v1, v2, v3;
			v0 = m_vGeomObj[i].get()->m_matCalculation.m[0];
			v1 = m_vGeomObj[i].get()->m_matCalculation.m[1];
			v2 = m_vGeomObj[i].get()->m_matCalculation.m[2];
			D3DXVec3Cross(&v3, &v1, &v2);
			if (D3DXVec3Dot(&v3, &v0) < 0.0f)
			{
				D3DXMATRIX matW;
				D3DXMatrixScaling(&matW, -1.0f, -1.0f, -1.0f);
				D3DXMatrixMultiply(&m_vGeomObj[i].get()->m_matCalculation,
					&m_vGeomObj[i].get()->m_matCalculation, &matW);
			}
		}
		else {
			m_vGeomObj[i].get()->m_matCalculation = matCalc = matWldScale * matWldRotate * matWldTrans;
			int iTest = 0;

			// 인버스 매트릭스 확인 코드.
			D3DXVECTOR3 v0, v1, v2, v3;
			v0 = m_vGeomObj[i].get()->m_matCalculation.m[0];
			v1 = m_vGeomObj[i].get()->m_matCalculation.m[1];
			v2 = m_vGeomObj[i].get()->m_matCalculation.m[2];
			D3DXVec3Cross(&v3, &v1, &v2);
			if (D3DXVec3Dot(&v3, &v0) < 0.0f)
			{
				D3DXMATRIX matW;
				D3DXMatrixScaling(&matW, -1.0f, -1.0f, -1.0f);
				D3DXMatrixMultiply(&m_vGeomObj[i].get()->m_matCalculation,
					&m_vGeomObj[i].get()->m_matCalculation, &matW);
			}
		}
	}
			//}
			
		//}

		//최종행렬.


	
}
void		GAseModel::SingleAniFrame() {

	D3DXMATRIX matWldTrans;
	D3DXMATRIX matWldRotate;
	D3DXMATRIX matWldScale;
	D3DXMatrixIdentity(&matWldTrans);
	D3DXMatrixIdentity(&matWldRotate);
	D3DXMatrixIdentity(&matWldScale);

	m_fTickFrame += g_fSecPerFrame * m_fFrameSpeed *m_fTickPerFrame;
	if (m_fTickFrame >= m_fLastFrame * m_fTickPerFrame /*마지막 프레임 틱수*/)
	{
		m_fTickFrame = 0.0f;
	}


	if (m_vGeomObj[0].get()->m_bHasAniTrack) {

		//Translation
		if (m_vGeomObj[0].get()->m_vPosTrack.size() != 0) {

			GAnimTrack* pStartTrack = NULL;
			GAnimTrack* pEndTrack = NULL;

			//현재 Tick이 어디인지 찾자.
			GetAnimationTrack(m_fTickFrame, &pStartTrack, &pEndTrack, ANITRACK_TYPE_POS);

			//애니메이션 보간.
			D3DXVECTOR3 vResultVector;
			D3DXVECTOR3 vP1 = pStartTrack->vecVector;
			D3DXVECTOR3 vP2 = pEndTrack->vecVector;

			float fTValue = (m_fTickFrame - pStartTrack->iTick) / (pEndTrack->iTick - pStartTrack->iTick);

			D3DXVec3Lerp(&vResultVector, &vP1, &vP2, fTValue);

			//T행렬 값 대입
			matWldTrans._41 = vResultVector.x;
			matWldTrans._42 = vResultVector.y;
			matWldTrans._43 = vResultVector.z;
		}

		//Rotation
		if (m_vGeomObj[0].get()->m_vRotTrack.size() != 0) {
			GAnimTrack* pStartTrack = NULL;
			GAnimTrack* pEndTrack = NULL;
			D3DXQUATERNION qR;

			//현재 Tick이 어디인지 찾자.
			GetAnimationTrack(m_fTickFrame, &pStartTrack, &pEndTrack, ANITRACK_TYPE_ROT);


			//사원수간의 보간..
			if (pStartTrack == NULL) {
				qR = m_vGeomObj[0].get()->m_qRotation;
				float fTValue = (m_fTickFrame - 0) / (pEndTrack->iTick - 0);
				D3DXQuaternionSlerp(&qR, &qR, &pEndTrack->qRotate, fTValue);
			}
			else if (pEndTrack == NULL) {
				qR = m_vGeomObj[0].get()->m_vRotTrack[m_vGeomObj[0].get()->m_vRotTrack.size() - 1].get()->qRotate;
				float fTValue = ((m_fTickFrame - pStartTrack->iTick) / (m_fFrameSpeed*m_fTickPerFrame));
				D3DXQuaternionSlerp(&qR, &qR, &qR, fTValue);
			}
			else {
				qR = pStartTrack->qRotate;
				float fTValue = (m_fTickFrame - pStartTrack->iTick) / (pEndTrack->iTick - pStartTrack->iTick);
				D3DXQuaternionSlerp(&qR, &qR, &pEndTrack->qRotate, fTValue);
			}

			//사원수에서 행렬로 변환.
			D3DXMatrixRotationQuaternion(&matWldRotate, &qR);// 사원수에서 행렬로 변환
		}

		//Scale
		if (m_vGeomObj[0].get()->m_vSclTrack.size() != 0) {

			GAnimTrack* pStartTrack = NULL;
			GAnimTrack* pEndTrack = NULL;

			D3DXMATRIX matScaleRot, matInvScaleRot;
			D3DXMatrixIdentity(&matScaleRot);
			D3DXMatrixIdentity(&matInvScaleRot);

			D3DXQUATERNION qS;
			float fStartTick = 0.0f, fEndTick = 0.0f;
			D3DXVECTOR3 vScale(m_vGeomObj[0].get()->m_matWldScale._11, m_vGeomObj[0].get()->m_matWldScale._22, m_vGeomObj[0].get()->m_matWldScale._33);

			//현재 Tick이 어디인지 찾자.
			GetAnimationTrack(m_fTickFrame, &pStartTrack, &pEndTrack, ANITRACK_TYPE_SCL);

			//신축트랙 보간
			if (pStartTrack == NULL) {
				//vScale = m_vGeomObj[0].get()->m_vecTM_SCALE;

				D3DXQuaternionRotationAxis(&qS, &m_vGeomObj[0].get()->m_vecTM_SCALE_AXIS, m_vGeomObj[0].get()->m_fTM_SCALEAXISANG);

				fStartTick = 0.0f;
				fEndTick = pEndTrack->iTick;
			}
			else if (pEndTrack == NULL) {
				vScale = pStartTrack->vecVector;
				qS = pStartTrack->qRotate;

				fStartTick = pStartTrack->iTick;

				fEndTick = pStartTrack->iTick + (m_fFrameSpeed*m_fTickPerFrame);
			}
			else {
				vScale = pStartTrack->vecVector;
				qS = pStartTrack->qRotate;

				fStartTick = pStartTrack->iTick;
				fEndTick = pEndTrack->iTick;


			}
			float fTValue = (m_fTickFrame - fStartTick) / (fEndTick - fStartTick);

			D3DXVec3Lerp(&vScale, &vScale, &pEndTrack->vecVector, fTValue);
			D3DXQuaternionSlerp(&qS, &qS, &pEndTrack->qRotate, fTValue);

			//사원수 -> 행렬로 변환등...
			D3DXMatrixScaling(&matWldScale, vScale.x, vScale.y, vScale.z);
			D3DXMatrixRotationQuaternion(&matScaleRot, &qS);
			D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot);

			matWldScale = matInvScaleRot * matWldScale * matScaleRot;



		}

		m_vGeomObj[0].get()->m_matCalculation = matWldScale
												* matWldRotate
												* matWldTrans;
	}
}
bool		GAseModel::Frame() {

	if (m_vGeomObj.size() == 1)
		SingleAniFrame();
	else
		MultiAniFrame();

	return true;
};

bool		GAseModel::SingleRender(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj) {

	//
	// Update variables
	//
	ConstantBuffer cb;

	D3DXMATRIX	  matTemp;
	D3DXMatrixIdentity(&matTemp);
	matTemp = m_vGeomObj[0].get()->m_matCalculation * *matWorld;

	D3DXMatrixTranspose(&cb.mWorld, &matTemp);
	D3DXMatrixTranspose(&cb.mView, matView);
	D3DXMatrixTranspose(&cb.mProjection, matProj);

	g_pImmediateContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, NULL, &cb, 0, 0);



	//
	// Renders a triangle
	//

	g_pImmediateContext->VSSetShader(m_pVertexShader.Get(), NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	g_pImmediateContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);
	g_pImmediateContext->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());



	if (m_vMaterial[0]->m_vSubMaterial.size() == 0) {

		g_pImmediateContext->PSSetShaderResources(0, 1, m_vMaterial[0]->m_pTextureRV.GetAddressOf());
		g_pImmediateContext->DrawIndexed(m_vGeomObj[0].get()->m_vObj[0]->m_vPnctVertex.size(), 0, 0);
	}
	else {
		for (int i = 0; i < m_vMaterial[0]->m_vSubMaterial.size(); i++) {

			//Set vertex buffer

			UINT stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;
			g_pImmediateContext->IASetVertexBuffers(0, 1, m_vGeomObj[0].get()->m_vObj[i]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);
			g_pImmediateContext->PSSetShaderResources(0, 1, m_vMaterial[0]->m_vSubMaterial[i]->m_pTextureRV.GetAddressOf());
			g_pImmediateContext->IASetIndexBuffer(m_vGeomObj[0].get()->m_vObj[i]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			g_pImmediateContext->DrawIndexed(m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size(), 0, 0);
			//g_pImmediateContext->Draw(m_vObj[i]->m_vPnctVertex.size(), 0);
		}
	}
	return true;
}
bool		GAseModel::MultiRender(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj) {
	//
	// Update variables
	//

	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.mView, matView);
	D3DXMatrixTranspose(&cb.mProjection, matProj);


	//
	// Renders a triangle
	//

	g_pImmediateContext->VSSetShader(m_pVertexShader.Get(), NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	g_pImmediateContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);
	g_pImmediateContext->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());

	for (int i = 0; i < m_vGeomObj.size(); i++) {

		if (m_vGeomObj[i]->m_bUsed == false)
			continue;

		D3DXMATRIX	  matTemp;
		D3DXMatrixIdentity(&matTemp);


		matTemp = m_vGeomObj[i].get()->m_matCalculation *  *matWorld;
		

		//matTemp = m_vGeomObj[i].get()->m_matWorld * *matWorld;

		D3DXMatrixTranspose(&cb.mWorld, &matTemp);
		g_pImmediateContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, NULL, &cb, 0, 0);


		for (int j = 0; j < m_vGeomObj[i]->m_vObj.size(); j++) {

			//Set vertex buffer

			UINT stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;
			g_pImmediateContext->IASetVertexBuffers(0, 1, m_vGeomObj[i].get()->m_vObj[j]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);
			g_pImmediateContext->PSSetShaderResources(0, 1, m_vMaterial[m_vGeomObj[i].get()->m_iMaterial_Ref]->m_vSubMaterial[j]->m_pTextureRV.GetAddressOf());
			g_pImmediateContext->IASetIndexBuffer(m_vGeomObj[i].get()->m_vObj[j]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			//g_pImmediateContext->DrawIndexed(m_vGeomObj[0].get()->m_vObj[i]->m_vPnctVertex.size(), 0, 0);
			g_pImmediateContext->Draw(m_vGeomObj[i].get()->m_vObj[j]->m_vPnctVertex.size(), 0);

		}
	}
	return true;

}
bool		GAseModel::Render(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj) {

	if (m_vGeomObj.size() == 1)
		SingleRender(matWorld,  matView,  matProj);
	else
		MultiRender(matWorld,  matView,  matProj);


	return true;
};
bool		GAseModel::Release() {

	return true;
};