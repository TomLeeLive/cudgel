#include "_stdafx.h"

bool		GAseModel::Init() {

	HRESULT hr = S_OK;


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
	hr = CompileShaderFromFile(L"Tutorial04.fx", "VS", "vs_4_0", &pVSBlob);
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
	hr = CompileShaderFromFile(L"Tutorial04.fx", "PS", "ps_4_0", &pPSBlob);
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



	
	if (m_vMaterial[0]->m_iSubMaterial == 0) {

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;


		hr = g_pd3dDevice->CreateBuffer(&bd, NULL, m_vObj[0]->m_pConstantBuffer.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Load the Texture
		hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/flagstone.bmp", NULL, NULL, m_vMaterial[0]->m_pTextureRV.GetAddressOf(), NULL);
		if (FAILED(hr))
			return hr;

		// Create vertex buffer
		PNCT_VERTEX* vertices;
		vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * m_vObj[0]->m_vPnctVertex.size());

		for (int j = 0; j < m_vObj[0]->m_vPnctVertex.size(); ++j) {

			vertices[j] = m_vObj[0]->m_vPnctVertex[j];

		}

		//D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(PNCT_VERTEX) * m_vObj[0]->m_vPnctVertex.size();//m_iPosCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;
		hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vObj[0]->m_pVertexBuffer.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Set vertex buffer
		UINT stride = sizeof(PNCT_VERTEX);
		UINT offset = 0;
		g_pImmediateContext->IASetVertexBuffers(0, 1, m_vObj[0]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);


		// Create index buffer
		WORD* indices;
		indices = (WORD *)malloc(sizeof(WORD) * m_vObj[0]->m_vPnctVertex.size());

		for (int j = 0; j < m_vObj[0]->m_vPnctVertex.size(); j++) {
			indices[j] = j;
		}


		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * m_vObj[0]->m_vPnctVertex.size();        // 36 vertices needed for 12 triangles in a triangle list
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;
		hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vObj[0]->m_pIndexBuffer.GetAddressOf());
		if (FAILED(hr))
			return hr;

		// Set index buffer
		g_pImmediateContext->IASetIndexBuffer(m_vObj[0]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		// Set primitive topology
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);








		delete[] indices;
		delete[] vertices;



	}
	else {

		for (int i = 1; i < m_vMaterial[0]->m_iSubMaterial+1; i++) {

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ConstantBuffer);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			hr = g_pd3dDevice->CreateBuffer(&bd, NULL, m_vObj[i]->m_pConstantBuffer.GetAddressOf());
			if (FAILED(hr))
				return hr;




#ifdef TESTTEST
			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/flagstone.bmp", NULL, NULL, m_vSubMaterial[i-1]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))
				return hr;
#else
			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/0_st02_sc00_g04.dds", NULL, NULL,  m_vMaterial[0]->m_vSubMaterial[0]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))																					    
				return hr;																					    
																											    
			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/0_st02_sc00_g00.dds", NULL, NULL,  m_vMaterial[0]->m_vSubMaterial[1]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))																					    
				return hr;																					    
			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/0_st02_sc00_g01.dds", NULL, NULL,  m_vMaterial[0]->m_vSubMaterial[2]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))																					    
				return hr;																					    
			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/0_st02_sc00_g03.dds", NULL, NULL,  m_vMaterial[0]->m_vSubMaterial[3]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))																					    
				return hr;																					    
			hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/0_st02_sc00_g02.dds", NULL, NULL,  m_vMaterial[0]->m_vSubMaterial[4]->m_pTextureRV.GetAddressOf(), NULL);
			if (FAILED(hr))
				return hr;
#endif 

		//}


		//for (int i = 1; i < m_vMaterial[0]->m_iSubMaterial + 1; i++) {

			// Create vertex buffer
			PNCT_VERTEX* vertices;
			vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * m_vObj[i]->m_vPnctVertex.size());

			for (int j = 0; j < m_vObj[i]->m_vPnctVertex.size(); ++j) {

				vertices[j] = m_vObj[i]->m_vPnctVertex[j];

			}

			//D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(PNCT_VERTEX) * m_vObj[i]->m_vPnctVertex.size();//m_iPosCount;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = vertices;
			hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, m_vObj[i]->m_pVertexBuffer.GetAddressOf());
			if (FAILED(hr))
				return hr;

			// Set vertex buffer
			UINT stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;
			g_pImmediateContext->IASetVertexBuffers(0, 1, m_vObj[i]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);


			// Create index buffer
			WORD* indices;
			indices = (WORD *)malloc(sizeof(WORD) * m_vObj[i]->m_vPnctVertex.size());

			for (int j = 0; j < m_vObj[i]->m_vPnctVertex.size(); j++) {
				indices[j] = j;
			}


			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(WORD) * m_vObj[i]->m_vPnctVertex.size();        // 36 vertices needed for 12 triangles in a triangle list
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = indices;
			hr = g_pd3dDevice->CreateBuffer(&bd, &InitData,m_vObj[i]->m_pIndexBuffer.GetAddressOf());
			if (FAILED(hr))
				return hr;

			// Set index buffer
			g_pImmediateContext->IASetIndexBuffer(m_vObj[i]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

			// Set primitive topology
			g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);








			delete[] indices;
			delete[] vertices;

	}


	
	}


	


	return true;
};
bool		GAseModel::Frame(D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj) {




	//
	// Update variables
	//
	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.mWorld, matWorld);
	D3DXMatrixTranspose(&cb.mView, matView);
	D3DXMatrixTranspose(&cb.mProjection, matProj);

	if (m_vMaterial[0]->m_iSubMaterial == 0) {
		g_pImmediateContext->UpdateSubresource(m_vObj[0]->m_pConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	}
	else {
		for (int i = 1; i < m_vMaterial[0]->m_iSubMaterial+1; i++) {
			g_pImmediateContext->UpdateSubresource(m_vObj[i]->m_pConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
		}
	}



	return true;
};

bool		GAseModel::Render() {

	//
	// Renders a triangle
	//


	if (m_vMaterial[0]->m_iSubMaterial == 0) {

		g_pImmediateContext->VSSetShader(m_pVertexShader.Get(), NULL, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, m_vObj[0]->m_pConstantBuffer.GetAddressOf());
		g_pImmediateContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);

		g_pImmediateContext->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());
		g_pImmediateContext->PSSetShaderResources(0, 1, m_vMaterial[0]->m_pTextureRV.GetAddressOf());
		g_pImmediateContext->DrawIndexed(m_vObj[0]->m_vPnctVertex.size(), 0, 0);
	}
	else {
		for (int i = 1; i < m_vMaterial[0]->m_iSubMaterial + 1; i++) {

			// Set vertex buffer
			UINT stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;
			g_pImmediateContext->IASetVertexBuffers(0, 1, m_vObj[i]->m_pVertexBuffer.GetAddressOf(), &stride, &offset);

			g_pImmediateContext->VSSetShader(m_pVertexShader.Get(), NULL, 0);
			g_pImmediateContext->VSSetConstantBuffers(0, 1, m_vObj[i]->m_pConstantBuffer.GetAddressOf());
			g_pImmediateContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);

			g_pImmediateContext->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());

			g_pImmediateContext->PSSetShaderResources(0, 1, m_vMaterial[0]->m_vSubMaterial[i-1]->m_pTextureRV.GetAddressOf());
			//int temp = m_vObj[i]->m_vPnctVertex.size();
			g_pImmediateContext->IASetIndexBuffer(m_vObj[i]->m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			g_pImmediateContext->DrawIndexed(m_vObj[i]->m_vPnctVertex.size(), 0, 0);
				// 36 vertices needed for 12 triangles in a triangle list
		}
	}


	return true;
};
bool		GAseModel::Release() {

	//if (m_vObj[0]->m_pVertexBuffer.Get()) m_vObj[0]->m_pVertexBuffer.Get()->Release();
	//if (m_vObj[0]->m_pIndexBuffer.Get()) m_vObj[0]->m_pIndexBuffer.Get()->Release();
	//if (m_vMaterial[0]->m_pTextureRV.Get()) m_vMaterial[0]->m_pTextureRV.Get()->Release();

	//if (m_vObj[0]->m_pConstantBuffer.Get()) m_vObj[0]->m_pConstantBuffer.Get()->Release();
	//if (m_pVertexLayout.Get()) m_pVertexLayout.Get()->Release();
	//if (m_pVertexShader.Get()) m_pVertexShader.Get()->Release();
	//if (m_pPixelShader.Get()) m_pPixelShader.Get()->Release();
	//if (m_pSamplerLinear.Get()) m_pSamplerLinear.Get()->Release();

	return true;
};