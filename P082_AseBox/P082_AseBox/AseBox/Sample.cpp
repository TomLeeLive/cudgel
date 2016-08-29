#include "Sample.h"


int		Sample::GetMeshDataFromFile(GAseObj* aseobj) {


	int iSize = sizeof(g_pAseMeshTokens) / sizeof(g_pAseMeshTokens[0]);

	while (!feof(m_Parser.m_pStream))
	{
		_fgetts(m_Parser.m_pBuffer, 256, m_Parser.m_pStream);
		_stscanf(m_Parser.m_pBuffer, _T("%s"), m_Parser.m_pString);


		for (int i = 0; i < iSize; i++) {


			if (!_tcsicmp(m_Parser.m_pString, g_pAseMeshTokens[i]))
			{
				switch (i)
				{

				case MESH_NUMVERTEX:
				{
					m_Parser.GetData(&(aseobj->m_iPosCount), INT_DATA);
					break;
				}
				break;
				case MESH_NUMFACES:
				{
					m_Parser.GetData(&(aseobj->m_iFaceCount), INT_DATA);
				}
				break;
				case MESH_VERTEX_LIST:
				{
					D3DXVECTOR3 pPos;
					for (int i = 0; i < aseobj->m_iPosCount; i++) {
						//pPos = new D3DXVECTOR3;
						m_Parser.GetDataFromFileLoop(L"*MESH_VERTEX", &pPos, MESH_VERTEX_DATA);
						aseobj->m_vPosList.push_back(pPos);
					}
				}
				break;
				case MESH_FACE_LIST:
				{
					ST_INT_THREE stMeshFace;
					for (int i = 0; i < aseobj->m_iFaceCount; i++) {
						m_Parser.GetDataFromFileLoop(L"*MESH_FACE", &stMeshFace, MESH_FACE_DATA);
						aseobj->m_vIndex.push_back(stMeshFace.index1);
						aseobj->m_vIndex.push_back(stMeshFace.index2);
						aseobj->m_vIndex.push_back(stMeshFace.index3);
					}
				}
				break;
				case MESH_NUMTVERTEX:
				{
					m_Parser.GetData(&(aseobj->m_iTexVerCount), INT_DATA);
				}
				break;
				case MESH_TVERTLIST:
				{
					D3DXVECTOR3 vUVData; float fTemp;
					for (int i = 0; i < aseobj->m_iTexVerCount; i++) {
						m_Parser.GetDataFromFileLoop(L"*MESH_TVERT", &vUVData, MESH_VERTEX_DATA);

						fTemp = vUVData.z;
						vUVData.z = vUVData.y;
						vUVData.y = fTemp;

						vUVData.y = 1.0f - vUVData.y;

						aseobj->m_vTexList.push_back(vUVData);
					}
				}
				break;
				case MESH_NUMTVFACES:
				{
					m_Parser.GetData(&(aseobj->m_iTexFaceCount), INT_DATA);
				}
				break;
				case MESH_TFACELIST:
				{
					ST_INT_THREE stFaceList;
					for (int i = 0; i < aseobj->m_iTexFaceCount; i++) {
						m_Parser.GetDataFromFileLoop(L"*MESH_TFACE", &stFaceList, FACE_DATA);
						aseobj->m_vTextureIndex.push_back(stFaceList.index1);
						aseobj->m_vTextureIndex.push_back(stFaceList.index2);
						aseobj->m_vTextureIndex.push_back(stFaceList.index3);
					}
				}
				break;
				case MESH_NUMCVERTEX:
				{
					m_Parser.GetData(&(aseobj->m_iColorVerCount), INT_DATA);
				}
				break;
				case MESH_NORMALS:
				{
					D3DXVECTOR3 vNormal;
					for (int i = 0; i < aseobj->m_iTexFaceCount; i++) {
						m_Parser.GetDataFromFileLoop(L"*MESH_FACENORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vFaceNormal.push_back(vNormal);
						m_Parser.GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vNorList.push_back(vNormal);
						m_Parser.GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vNorList.push_back(vNormal);
						m_Parser.GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vNorList.push_back(vNormal);
					}
				}
				break;

				}
			}
		}
		//fgetc(m_Parser.m_pStream);
	}

	return -1;
}
int		Sample::GetObjDataFromFile() {

	//GAseObj* aseobj = new GAseObj;
	auto aseobj = make_shared<GAseObj>();

	int iSize = sizeof(g_pAseObjTokens) / sizeof(g_pAseObjTokens[0]);

	while (!feof(m_Parser.m_pStream))
	{
		_fgetts(m_Parser.m_pBuffer, 256, m_Parser.m_pStream);
		_stscanf(m_Parser.m_pBuffer, _T("%s"), m_Parser.m_pString);


		for (int i = 0; i < iSize; i++) {


			if (!_tcsicmp(m_Parser.m_pString, g_pAseObjTokens[i]))
			{
				switch (i)
				{

				case NODE_NAME:
				{

					m_Parser.GetData(&(aseobj->m_szName), STRING_DATA);
				}
				break;
				case NODE_TM:
				{
					D3DXVECTOR3 vecROW0;
					D3DXVECTOR3 vecROW1;
					D3DXVECTOR3 vecROW2;
					D3DXVECTOR3 vecROW3;
					m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[0], &vecROW0, VERTEX_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[1], &vecROW1, VERTEX_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[2], &vecROW2, VERTEX_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseNodeTmTokens[3], &vecROW3, VERTEX_DATA);

					D3DXMatrixIdentity(&aseobj->m_matWorld);

					aseobj->m_matWorld._11 = vecROW0.x; aseobj->m_matWorld._12 = vecROW0.y; aseobj->m_matWorld._13 = vecROW0.z;
					aseobj->m_matWorld._31 = vecROW1.x; aseobj->m_matWorld._32 = vecROW1.y; aseobj->m_matWorld._33 = vecROW1.z;
					aseobj->m_matWorld._21 = vecROW2.x; aseobj->m_matWorld._22 = vecROW2.y; aseobj->m_matWorld._23 = vecROW2.z;
					aseobj->m_matWorld._41 = vecROW3.x; aseobj->m_matWorld._42 = vecROW3.y; aseobj->m_matWorld._43 = vecROW3.z;

				}
				break;
				case MESH:
				{

					GetMeshDataFromFile(aseobj.get());


				}
				break;

				}
			}
		}
		//fgetc(m_Parser.m_pStream);
	}

	m_stModel.m_vObj.push_back(aseobj);
	return -1;

}
int		Sample::GetDataFromFile() {

	int iSize = sizeof(g_pAseItems) / sizeof(g_pAseItems[0]);

	while (!feof(m_Parser.m_pStream))
	{
		_fgetts(m_Parser.m_pBuffer, 256, m_Parser.m_pStream);
		_stscanf(m_Parser.m_pBuffer, _T("%s"), m_Parser.m_pString);


		for (int i = 0; i < iSize; i++) {

			if (!_tcsicmp(m_Parser.m_pString, g_pAseItems[i]))
			{
				switch (i)
				{
				case SCENE:
				{
					m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[0], &m_stModel.m_stScene.m_szName, STRING_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[1], &m_stModel.m_stScene.m_iFrame, INT_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[2], &m_stModel.m_stScene.m_iLastFrame, INT_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[3], &m_stModel.m_stScene.m_iFrameSpeed, INT_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseSceneTokens[4], &m_stModel.m_stScene.m_iTicksPerFrame, INT_DATA);
				}
				break;
				case MATERIALLIST:
				{
					//GAseMaterial* material = new GAseMaterial;
					auto material = make_shared<GAseMaterial>();
					m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[0], &(material->m_szName), STRING_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[1], &(material->m_vecAmbient), VERTEX_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[2], &(material->m_vecDiffuse), VERTEX_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[3], &(material->m_vecSpecular), VERTEX_DATA);
					m_Parser.GetDataFromFileLoop(g_pAseMaterialTokens[4], &(material->m_szMapDiffuse), STRING_DATA);
					m_stModel.m_vMaterial.push_back(material);
				}
				break;
				case GEOMOBJECT:
				{
					GetObjDataFromFile();
				}
				break;
				}
			}
		}
		//fgetc(m_Parser.m_pStream);
	}

	return -1;
}
void    Sample::SetPnctData(){

		D3DXVECTOR3		vp;
		D3DXVECTOR3		vn;
		D3DXVECTOR4		vc;
		D3DXVECTOR2     vt;




		// Create index buffer
		WORD* indices;
		indices = (WORD *)malloc(sizeof(WORD) * m_stModel.m_vObj[0]->m_iFaceCount * 3);


		for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {

			if (i == 0 || i % 3 == 0) {
				indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i]) };
			}
			else if (i == 1 || i % 3 == 1) {
				indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i + 1]) };
			}
			else if (i == 2 || i % 3 == 2) {
				indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i - 1]) };
			}
		}


		WORD* Texindices;
		Texindices = (WORD *)malloc(sizeof(WORD) * m_stModel.m_vObj[0]->m_iFaceCount * 3);


		for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {

			if (i == 0 || i % 3 == 0) {
				Texindices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vTextureIndex[i]) };
			}
			else if (i == 1 || i % 3 == 1) {
				Texindices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vTextureIndex[i + 1]) };
			}
			else if (i == 2 || i % 3 == 2) {
				Texindices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vTextureIndex[i - 1]) };
			}
		}

		// Create vertex buffer
		PNCT_VERTEX* vertices;
		vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * m_stModel.m_vObj[0]->m_iFaceCount * 3);


		for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {
			
			vp = m_stModel.m_vObj[0]->m_vPosList[indices[i]];
			vn = m_stModel.m_vObj[0]->m_vNorList[indices[i]];
			vc = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			vt = D3DXVECTOR2(m_stModel.m_vObj[0]->m_vTexList[Texindices[i]].x, m_stModel.m_vObj[0]->m_vTexList[Texindices[i]].y);
			m_stModel.m_vObj[0]->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
		}

		delete[] Texindices;
		delete[] indices;
		delete[] vertices;
}
bool	Sample::Init() {
	m_Parser.OpenStream(L"data/BOX.ASE");

	GetDataFromFile();

	m_Parser.CloseStream();

	SetPnctData();

	HRESULT hr = S_OK;

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
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
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
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

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
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

#ifdef TESTTEST
	// Create index buffer
	WORD* indices;
	indices = (WORD *)malloc(sizeof(WORD) * m_stModel.m_vObj[0]->m_iFaceCount * 3);


	for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {

		if (i == 0 || i % 3 == 0) {
			indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i]) };
		}
		else if (i == 1 || i % 3 == 1) {
			indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i + 1]) };
		}
		else if (i == 2 || i % 3 == 2) {
			indices[i] = { (WORD)(m_stModel.m_vObj[0]->m_vIndex[i - 1]) };
		}
	}

	// Create vertex buffer
	SimpleVertex* vertices;
	vertices = (SimpleVertex *)malloc(sizeof(SimpleVertex) * m_stModel.m_vObj[0]->m_iFaceCount * 3);


	for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {
		//vertices[i] = { m_stModel.m_vObj[0]->m_vPosList[i], D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f) };
		vertices[i] = { m_stModel.m_vObj[0]->m_vPosList[indices[i]], D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f) };

	}
#endif

	// Create vertex buffer
	PNCT_VERTEX* vertices;
	vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * m_stModel.m_vObj[0]->m_iFaceCount * 3);

	for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {

		vertices[i] = m_stModel.m_vObj[0]->m_vPnctVertex[i];

	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(PNCT_VERTEX) * m_stModel.m_vObj[0]->m_iFaceCount * 3;//m_iPosCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(PNCT_VERTEX);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// Create index buffer
	WORD* indices;
	indices = (WORD *)malloc(sizeof(WORD) * m_stModel.m_vObj[0]->m_iFaceCount * 3);

	for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {
		indices[i] = i;
	}

#ifdef TESTTEST
	//Index buffer re-ordering
	for (int i = 0; i < m_stModel.m_vObj[0]->m_iFaceCount * 3; i++) {
		indices[i] = i;
	}
#endif
	


	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * m_stModel.m_vObj[0]->m_iFaceCount * 3;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// Set index buffer
	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;





	// Load the Texture
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"data/flagstone.bmp", NULL, NULL, &g_pTextureRV, NULL);
	if (FAILED(hr))
		return hr;

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
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr))
		return hr;





	// Initialize the world matrix
	D3DXMatrixIdentity(&g_World);

	// Initialize the view matrix
	D3DXVECTOR3 Eye = D3DXVECTOR3(0.0f, 1.0f, -7.0f);
	D3DXVECTOR3 At = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_View, &Eye, &At, &Up);

	// Initialize the projection matrix
	D3DXMatrixPerspectiveFovLH(&g_Projection, XM_PIDIV2, m_iWindowWidth / (FLOAT)m_iWindowHeight, 0.01f, 1000.0f);



	delete[] indices;
	delete[] vertices;
	return true;
};
bool	Sample::Frame() {


	return true;
};
bool	Sample::Render() {
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
	D3DXMatrixRotationY(&g_World, t);

	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	g_pImmediateContext->ClearRenderTargetView(GetRenderTargetView(), ClearColor);

	//
	// Update variables
	//
	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.mWorld, &g_World);
	D3DXMatrixTranspose(&cb.mView, &g_View);
	D3DXMatrixTranspose(&cb.mProjection, &g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);

	//
	// Renders a triangle
	//
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);

	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->DrawIndexed(36, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list

													   //
													   // Present our back buffer to our front buffer
													   //
	m_pSwapChain->Present(0, 0);

	return true;
};
bool	Sample::Release() {

	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();


	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pTextureRV) g_pTextureRV->Release();
	return true;
};

Sample::Sample()
{
}


Sample::~Sample()
{



	//for (int i = 0; i < m_stModel.m_vObj.size(); i++)
	//{
	//	for (int j = 0; j < (m_stModel.m_vObj[i])->m_vPosList.size(); j++)
	//	{
	//		delete (m_stModel.m_vObj[i])->m_vPosList[j];
	//	}
	//	
	//}
	//for (int i = 0; i < m_stModel.m_vObj.size(); i++)
	//{
	//	delete &(m_stModel.m_vObj[i]);
	//}
	//for (int i = 0; i < m_stModel.m_vMaterial.size(); i++)
	//{
	//	delete &(m_stModel.m_vMaterial[i]);
	//}

}
GBASIS_RUN(L"Ase Box");