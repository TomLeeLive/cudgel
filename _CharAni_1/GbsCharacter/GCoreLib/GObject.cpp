#include "GObject.h"
#include "GObjMgr.h"

bool GObject::Init()
{
	return true;
}
bool GObject::Load(ID3D11Device* pd3dDevice,
	const TCHAR* strFileName,
	const TCHAR* strShaderName, bool bThread)
{
	int iIndex = I_ObjMgr.Load(pd3dDevice, strFileName, strShaderName);
	if (iIndex >= 0)
	{
		m_pModelList.push_back(iIndex);		
		if ( !bThread && !Create(pd3dDevice, strShaderName))
		{
			I_ObjMgr.Delete(iIndex);
			return -1;
		}
	}
	return true;
}
bool GObject::Frame()
{
	for (int iObj = 0; iObj < m_pModelList.size(); iObj++)
	{
		GModel* pObj = I_ObjMgr.GetPtr(m_pModelList[iObj]);
		_ASSERT(pObj);
		/*static float fTime = 0.0f;
		fTime += g_fSecPerFrame;
		D3DXMatrixRotationY(&pObj->m_matControlWorld, fTime);*/
		pObj->Frame();
	}
	return true;
}

bool GObject::Render(ID3D11DeviceContext*    pContext)
{
	PreRender(pContext);
	for (int iObj = 0; iObj < m_pModelList.size(); iObj++)
	{
		GModel* pObj = I_ObjMgr.GetPtr(m_pModelList[iObj]);
		_ASSERT(pObj);
		for (DWORD dwObject = 0; dwObject < pObj->m_pMesh.size(); dwObject++)
		{
			GMesh* pMesh = pObj->m_pMesh[dwObject].get();
			SetMatrix(&pMesh->m_matCalculation, &m_matView, &m_matProj);
			pContext->UpdateSubresource(m_Object.g_pConstantBuffer.Get(), 0, NULL, &m_cbData, 0, 0);

			if (pMesh->m_pSubMesh.size() > 0)
			{
				for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
				{
					GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
					pContext->PSSetShaderResources(0, 1, pSubMesh->m_Object.g_pTextureSRV.GetAddressOf());
					pContext->DrawIndexed(pSubMesh->m_Object.m_iNumIndex,
						pSubMesh->m_Object.m_iBeginIB,
						pSubMesh->m_Object.m_iBeginVB);
				}
			}
			else
			{
				pContext->PSSetShaderResources(0, 1, pMesh->m_Object.g_pTextureSRV.GetAddressOf());
				pContext->DrawIndexed(pMesh->m_Object.m_iNumIndex,
					pMesh->m_Object.m_iBeginIB,
					pMesh->m_Object.m_iBeginVB);

			}
		}	
	}
	return true;
}
bool GObject::Release()
{
	m_pModelList.clear();
	return true;
}

bool GObject::ResetResource()
{
	return true;
}

HRESULT GObject::LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString)
{
	HRESULT hr = S_OK;
	GModel* pObj = I_ObjMgr.GetPtr(m_pModelList[m_pModelList.size() - 1]);
	for (DWORD dwObject = 0; dwObject < pObj->m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = pObj->m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_iTexIndex < 0) continue;
				pSubMesh->m_Object.g_pTextureSRV = I_Texture.GetPtr(pSubMesh->m_iTexIndex)->m_pTextureRV;
			}
		}
		else
		{
			if (pMesh->m_iTexIndex < 0) continue;
			pMesh->m_Object.g_pTextureSRV = I_Texture.GetPtr(pMesh->m_iTexIndex)->m_pTextureRV;
		}
	}
	return hr;
}
//HRESULT GObject::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pShaderFile)
//{
//	m_Object.g_pVertexShader.Attach(DX::LoadVertexShaderFile(pDevice, pShaderFile, m_Object.g_pVSBlob.GetAddressOf()));
//	m_Object.g_pPixelShader.Attach(DX::LoadPixelShaderFile(pDevice, pShaderFile));
//	m_Object.g_pGeometryShader.Attach(DX::LoadGeometryShaderFile(pDevice, pShaderFile, m_Object.g_pGSBlob.GetAddressOf()));
//	return S_OK;
//}
//HRESULT GObject::CreateConstantBuffer()
//{
//	m_cbData.Color = D3DXVECTOR4(1, 1, 1, 1);
//	m_Object.g_pConstantBuffer.Attach(DX::CreateConstantBuffer(m_pd3dDevice, &m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));	
//	return S_OK;
//}
HRESULT GObject::SetInputLayout()
{
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_Object.g_pInputlayout.Attach(DX::CreateInputlayout(m_pd3dDevice,
		m_Object.g_pVSBlob.Get()->GetBufferSize(),
		m_Object.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));	
	return hr;
}
HRESULT GObject::CreateVertexBuffer()
{
	GModel* pObj = I_ObjMgr.GetPtr(m_pModelList[m_pModelList.size()-1]);

	m_Object.g_pVertexBuffer.Attach(DX::CreateVertexBuffer(m_pd3dDevice,
		NULL,
		pObj->m_iMaxVertex,
		sizeof(PNCT_VERTEX)));

	m_Object.m_iVertexSize = sizeof(PNCT_VERTEX);

	size_t dstOffset = 0;
	size_t vbOffset = 0;

	for (DWORD dwObject = 0; dwObject < pObj->m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = pObj->m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_VertexArray.size() < 3) continue;

				pSubMesh->m_Object.m_iNumVertex = pSubMesh->m_VertexArray.size();
				pSubMesh->m_Object.m_iVertexSize = sizeof(PNCT_VERTEX);

				pSubMesh->m_Object.m_BoxVB.left = dstOffset;
				pSubMesh->m_Object.m_BoxVB.right = dstOffset + pSubMesh->m_Object.m_iNumVertex*sizeof(PNCT_VERTEX);
				pSubMesh->m_Object.m_BoxVB.top = 0; pSubMesh->m_Object.m_BoxVB.bottom = 1;
				pSubMesh->m_Object.m_BoxVB.front = 0; pSubMesh->m_Object.m_BoxVB.back = 1;

				g_pImmediateContext->UpdateSubresource(
					m_Object.g_pVertexBuffer.Get(), 0,
					&pSubMesh->m_Object.m_BoxVB, 
					(uint8_t*)&pSubMesh->m_VertexArray.at(0), 
					0, 0);

				
				/*g_pImmediateContext->CopySubresourceRegion(
					m_Object.g_pVertexBuffer.Get(), 0, iBeginPos, 0, 0, 
					(void*)&pSubMesh->m_VertexArray.at(0),
					0, &pSubMesh->m_Object.m_BoxVB);*/

				pSubMesh->m_Object.m_iBeginVB = vbOffset;
				vbOffset += pSubMesh->m_Object.m_iNumVertex;
				
				dstOffset = pSubMesh->m_Object.m_BoxVB.right;				
			}
		}
		else
		{
			if (pMesh->m_VertexArray.size() < 3) continue;
			pMesh->m_Object.m_iNumVertex = pMesh->m_VertexArray.size();
			pMesh->m_Object.m_iVertexSize = sizeof(PNCT_VERTEX);

			pMesh->m_Object.m_BoxVB.left = dstOffset;
			pMesh->m_Object.m_BoxVB.right = dstOffset + pMesh->m_Object.m_iNumVertex*sizeof(PNCT_VERTEX);
			pMesh->m_Object.m_BoxVB.top = 0; pMesh->m_Object.m_BoxVB.bottom = 1;
			pMesh->m_Object.m_BoxVB.front = 0; pMesh->m_Object.m_BoxVB.back = 1;

			g_pImmediateContext->UpdateSubresource(m_Object.g_pVertexBuffer.Get(), 0,
				&pMesh->m_Object.m_BoxVB, (void*)&pMesh->m_VertexArray.at(0), 0, 0);

			pMesh->m_Object.m_iBeginVB = vbOffset;			
			vbOffset += pMesh->m_Object.m_iNumVertex;

			dstOffset = pMesh->m_Object.m_BoxVB.right;
		}
	}
	return S_OK;
}
HRESULT	 GObject::CreateIndexBuffer()
{
	GModel* pObj = I_ObjMgr.GetPtr(m_pModelList[m_pModelList.size() - 1]);

	m_Object.g_pIndexBuffer.Attach(DX::CreateIndexBuffer(m_pd3dDevice,
		NULL,
		pObj->m_iMaxIndex,
		sizeof(DWORD)));

	size_t dstOffset = 0;
	size_t ibOffset = 0;
	for (DWORD dwObject = 0; dwObject < pObj->m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = pObj->m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_VertexArray.size() < 3) continue;

				pSubMesh->m_Object.m_iNumIndex = pSubMesh->m_IndexArray.size();
				pSubMesh->m_Object.m_BoxIB.left = dstOffset;
				pSubMesh->m_Object.m_BoxIB.right = dstOffset + pSubMesh->m_Object.m_iNumIndex*sizeof(DWORD);
				pSubMesh->m_Object.m_BoxIB.top = 0;		pSubMesh->m_Object.m_BoxIB.bottom = 1;
				pSubMesh->m_Object.m_BoxIB.front = 0;	pSubMesh->m_Object.m_BoxIB.back = 1;

				g_pImmediateContext->UpdateSubresource(m_Object.g_pIndexBuffer.Get(), 0,
					&pSubMesh->m_Object.m_BoxIB, (void*)&pSubMesh->m_IndexArray.at(0), 0, 0);

				pSubMesh->m_Object.m_iBeginIB = ibOffset;
				ibOffset += pSubMesh->m_Object.m_iNumIndex;
				dstOffset = pSubMesh->m_Object.m_BoxIB.right;
			}
		}
		else
		{
			if (pMesh->m_VertexArray.size() < 3) continue;
			pMesh->m_Object.m_iNumIndex = pMesh->m_IndexArray.size();
	
			pMesh->m_Object.m_BoxIB.left = dstOffset;
			pMesh->m_Object.m_BoxIB.right = dstOffset + pMesh->m_Object.m_iNumIndex*sizeof(DWORD);
			pMesh->m_Object.m_BoxIB.top = 0; pMesh->m_Object.m_BoxIB.bottom = 1;
			pMesh->m_Object.m_BoxIB.front = 0; pMesh->m_Object.m_BoxIB.back = 1;

			g_pImmediateContext->UpdateSubresource(m_Object.g_pIndexBuffer.Get(), 0,
				&pMesh->m_Object.m_BoxIB, (void*)&pMesh->m_IndexArray.at(0), 0, 0);

			pMesh->m_Object.m_iBeginIB = ibOffset;
			ibOffset += pMesh->m_Object.m_iNumIndex;
			dstOffset = pMesh->m_Object.m_BoxIB.right;
		}
	}
	return S_OK;
}
GObject::GObject(void)
{
}

GObject::~GObject(void)
{
}

GModel* GObject::SetActionFrame(TCHAR* pStrModel, DWORD dwStrat, DWORD dwEnd)
{
	GModel* pObj = NULL;
	for (int iObj = 0; iObj < m_pModelList.size(); iObj++)
	{
		pObj = I_ObjMgr.GetPtr(m_pModelList[iObj]);
		_ASSERT(pObj);
		if (!_tcsicmp(pObj->m_szName.c_str(), pStrModel))
		{
			pObj->m_iStartFrame = dwStrat;
			pObj->m_iLastFrame = dwEnd;
			pObj->m_fElapseTime = (float)dwStrat;
		}
	}
	return pObj;
}
GModel* GObject::SetActionFrame(DWORD dwIndex, DWORD dwStrat, DWORD dwEnd)
{
	GModel* pObj = I_ObjMgr.GetPtr(m_pModelList[dwIndex]);
	_ASSERT(pObj);
	if (pObj)
	{
		pObj->m_iStartFrame = dwStrat;
		pObj->m_iLastFrame = dwEnd;
		pObj->m_fElapseTime = (float)dwStrat;
	}
	return pObj;
}