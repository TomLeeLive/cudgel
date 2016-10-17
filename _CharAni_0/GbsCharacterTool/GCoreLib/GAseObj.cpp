#include "GAseObj.h"
#include "GTimer.h"
void GAseObj::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	if (pWorld != NULL)
	{
		m_matControlWorld = *pWorld;
		/*m_vCenterPos.x = pWorld->_41;
		m_vCenterPos.y = pWorld->_42;
		m_vCenterPos.z = pWorld->_43;*/
	}
	if (pView != NULL)
	{
		m_matView = *pView;
	}
	if (pProj != NULL)
	{
		m_matProj = *pProj;
	}
	D3DXMatrixTranspose(&m_cbData.matWorld, &m_matControlWorld);
	D3DXMatrixTranspose(&m_cbData.matView, &m_matView);
	D3DXMatrixTranspose(&m_cbData.matProj, &m_matProj);
}
bool GAseObj::Load(	ID3D11Device* pd3dDevice, 
					const TCHAR* szLoadName, 
					const TCHAR* pLoadShaderFile	)
{
	__int64 qwElapsedTimeA, qwElapsedTimeB, qwElapsedTimeC;
	if (m_Parser.Load(szLoadName))
	{
		CStopwatch stopwatch;
		int iObjectType = -1;
		int iNumTypes = sizeof(AseSections) / sizeof(AseSections[0]);
		while ((iObjectType = m_Parser.SearchTokenArray(iNumTypes, AseSections, m_Parser.m_dwMaxStringTable)) >= 0)
		{
			switch (iObjectType)
			{
			case SCENE:
			{
				if (!m_Parser.LoadScene(m_Scene))		return false;
				qwElapsedTimeA = stopwatch.Now();
			}break;
			case MATERIALLIST:
			{
				if (!m_Parser.LoadMaterialList(m_Material)) return false;
				qwElapsedTimeB = stopwatch.Now() - qwElapsedTimeA;
			}break;
			case GEOMOBJECT:
			{
				if (!m_Parser.LoadGeomObject())		return false;
			}break;
			case HELPEROBJECT:
			{
				if (!m_Parser.LoadHelperObject())		return false;
			}break;
			default:
				return false;
			}
		}
		stopwatch.Output(L"GAseObj::Load");
	}
	m_szDirName = m_Parser.m_szDirName;
	m_szName = m_Parser.m_szName;

	CStopwatch stopwatch;
	if (!Convert(pd3dDevice))
	{
		return false;
	}

	if (m_bSetLoadDraw && !Create(pd3dDevice, pLoadShaderFile))
	{
		m_Parser.Release();
		return false;
	}
	m_Parser.Release();
	stopwatch.Output(L"GAseObj::Load After");
	return true;
}
void GAseObj::Interpolate(GMesh* pMesh,
	D3DXMATRIX* matParents, float fFrameTick)
{
	// TM		= AnimMat * ParentTM;
	// AaniMat	= TM * Inverse(ParentTM)
	D3DXQUATERNION qR, qS;
	D3DXMatrixIdentity(&pMesh->m_matCalculation);
	D3DXMATRIX matAnim, matPos, matRotate, matScale;

	matRotate = pMesh->m_matWorldRotate;
	matPos = pMesh->m_matWorldTrans;
	matScale = pMesh->m_matWorldScale;

	D3DXQuaternionRotationMatrix(&qR, &matRotate);
	D3DXQuaternionRotationMatrix(&qS, &matScale);

	// fFrameTick = m_Scene.iFirstFrame * m_Scene.iTickPerFrame + CurFame;
	float fStartTick = m_Scene.iFirstFrame * m_Scene.iTickPerFrame;
	float fEndTick = 0.0f;

	GAnimTrack* pStartTrack = NULL;
	GAnimTrack* pEndTrack = NULL;
	if (pMesh->m_pRotTrack.size())
	{
		// pStartTrack를 찾을수 있으면
		if (GetAnimationTrack(fFrameTick, pMesh->m_pRotTrack, &pStartTrack, &pEndTrack))
		{
			qR = pStartTrack->qRotate;
			fStartTick = pStartTrack->iTick;
		}
		if (pEndTrack)
		{
			fEndTick = pEndTrack->iTick;
			D3DXQuaternionSlerp(&qR, &qR, &pEndTrack->qRotate, (fFrameTick - fStartTick) / (fEndTick - fStartTick));
		}
		D3DXMatrixRotationQuaternion(&matRotate, &qR);
	}

	pStartTrack = NULL;
	pEndTrack = NULL;

	D3DXVECTOR3 Trans(matPos._41, matPos._42, matPos._43);
	if (pMesh->m_pPosTrack.size())
	{
		// pStartTrack를 찾을수 있으면
		if (GetAnimationTrack(fFrameTick, pMesh->m_pPosTrack, &pStartTrack, &pEndTrack))
		{
			Trans = pStartTrack->vVector;
			fStartTick = pStartTrack->iTick;
		}
		if (pEndTrack)
		{
			fEndTick = pEndTrack->iTick;
			D3DXVec3Lerp(&Trans, &Trans, &pEndTrack->vVector, (fFrameTick - fStartTick) / (fEndTick - fStartTick));
		}

		D3DXMatrixTranslation(&matPos, Trans.x, Trans.y, Trans.z);
	}


	pStartTrack = NULL;
	pEndTrack = NULL;

	D3DXMATRIX matScaleRot, matInvScaleRot;
	D3DXVECTOR3 vScale(matScale._11, matScale._22, matScale._33);
	if (pMesh->m_pSclTrack.size())
	{
		// pStartTrack를 찾을수 있으면
		if (GetAnimationTrack(fFrameTick, pMesh->m_pSclTrack, &pStartTrack, &pEndTrack))
		{
			vScale = pStartTrack->vVector;
			qS = pStartTrack->qRotate;
			fStartTick = pStartTrack->iTick;
		}
		if (pEndTrack)
		{
			fEndTick = pEndTrack->iTick;
			D3DXVec3Lerp(&vScale, &vScale, &pEndTrack->vVector, (fFrameTick - fStartTick) / (fEndTick - fStartTick));
			D3DXQuaternionSlerp(&qS, &qS, &pEndTrack->qRotate, (fFrameTick - fStartTick) / (fEndTick - fStartTick));
		}
		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
		D3DXMatrixRotationQuaternion(&matScaleRot, &qS);
		D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot);
		matScale = matInvScaleRot  * matScale * matScaleRot;
	}

	pStartTrack = NULL;
	pEndTrack = NULL;

	float fCurAlpha, fNextAlpha, fOffSet;
	fCurAlpha = 0.0f;
	fNextAlpha = 0.0f;
	if (pMesh->m_pVisTrack.size())
	{
		// pStartTrack를 찾을수 있으면
		if (GetAnimationTrack(fFrameTick, pMesh->m_pVisTrack, &pStartTrack, &pEndTrack))
		{
			fCurAlpha = pStartTrack->vVector.x;
			fStartTick = pStartTrack->iTick;
		}
		if (pEndTrack)
		{
			fNextAlpha = pEndTrack->vVector.x;
			fEndTick = pEndTrack->iTick;

			fOffSet = (fFrameTick - fStartTick) / (fEndTick - fStartTick);
			fNextAlpha = (fNextAlpha - fCurAlpha)*fOffSet;
		}
		pMesh->m_fVisibility = (fCurAlpha + fNextAlpha);
	}
	else
	{
		pMesh->m_fVisibility = 1.0f;
	}

	D3DXMatrixMultiply(&matAnim, &matScale, &matRotate);
	matAnim._41 = matPos._41;
	matAnim._42 = matPos._42;
	matAnim._43 = matPos._43;
	// 최종 에미메이션 행렬을 완성한다.	
	D3DXMatrixMultiply(&pMesh->m_matCalculation, &matAnim, matParents);

	// 인버스 매트릭스 확인 코드.
	D3DXVECTOR3 v0, v1, v2, v3;
	v0 = pMesh->m_matCalculation.m[0];
	v1 = pMesh->m_matCalculation.m[1];
	v2 = pMesh->m_matCalculation.m[2];
	D3DXVec3Cross(&v3, &v1, &v2);
	if (D3DXVec3Dot(&v3, &v0) < 0.0f)
	{
		D3DXMATRIX matW;
		D3DXMatrixScaling(&matW, -1.0f, -1.0f, -1.0f);
		D3DXMatrixMultiply(&pMesh->m_matCalculation,
			&pMesh->m_matCalculation, &matW);
	}
}
// pEndTrack 트랙이 없으면 flase 리턴( 보간할 대상이 없을 때 )
bool GAseObj::GetAnimationTrack(float fFrame,
	vector<shared_ptr<GAnimTrack>> pTrackList,
	GAnimTrack** pStartTrack,
	GAnimTrack** pEndTrack)
{
	for (DWORD dwTrack = 0; dwTrack < pTrackList.size(); dwTrack++)
	{
		GAnimTrack *pTrack = pTrackList[dwTrack].get();
		_ASSERT(pTrack);
		// fFrame 보다 큰 Tick 트랙이 있다면 이전 트랙을 넘겨 주어야 하기 때문에 break한다.
		if (pTrack->iTick > fFrame)
		{
			*pEndTrack = pTrack;
			break;
		}
		// 현프레임보다 큰값이 없다면. 가장 시간경과가 오래된 것을 사용한다.
		*pStartTrack = pTrack;
	}
	return (*pStartTrack != NULL) ? true : false;
}
bool GAseObj::Init()
{
	return true;
}

bool GAseObj::Frame()
{
	m_fElapseTime += g_fSecPerFrame * 1.0f * m_Scene.iFrameSpeed * m_Scene.iTickPerFrame;
	if (m_fElapseTime >= m_Scene.iLastFrame* m_Scene.iTickPerFrame)
	{
		m_fElapseTime = m_Scene.iFirstFrame * m_Scene.iTickPerFrame;
	}

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		if (pMesh->m_bUsed == false) continue;
		if (pMesh->m_pParent)
			Interpolate(pMesh, &pMesh->m_pParent->m_matCalculation, m_fElapseTime);
		else
			Interpolate(pMesh, &mat, m_fElapseTime);
	}
	// 상속관계의 에니메이션 행렬을 구한 이후에는 각 메쉬당 에디터 행렬 적용( 이동 및 회전/신축 )
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		_ASSERT(pMesh);
		D3DXMatrixMultiply(&pMesh->m_matCalculation, &pMesh->m_matCalculation, &m_matControlWorld);
	}
	return true;
}
bool GAseObj::Render(ID3D11DeviceContext*    pContext)
{
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		//--------------------------------------------------------------------------------------
		// 월드행렬을 쉐이더로 전달
		//--------------------------------------------------------------------------------------
		pMesh->SetMatrix(&pMesh->m_matCalculation, &m_matView, &m_matProj);
		D3DXVECTOR4 vColor(1, 1, 1, pMesh->m_fVisibility);
		m_cbData.Color = vColor;

		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSubCount = 0; dwSubCount < pMesh->m_pSubMesh.size(); dwSubCount++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSubCount].get();
				if (pSubMesh->m_iNumFace <= 0) continue;
				pSubMesh->SetMatrix(&pMesh->m_matCalculation, &m_matView, &m_matProj);
				pSubMesh->Render(pContext);
			}
		}
		else
		{
			if (pMesh->m_iNumFace <= 0) continue;
			pMesh->Render(pContext);
		}
	}
	return true;
}
bool GAseObj::Release()
{
	return true;
}
bool GAseObj::Convert(ID3D11Device* pDevice)
{
	for (DWORD dwObject = 0; dwObject < m_Parser.m_pAseMesh.size(); dwObject++)
	{
		auto pMesh = make_shared<GMesh>();

		pMesh->m_matWorld = m_Parser.m_pAseMesh[dwObject]->m_matWorld;		// 월드행렬	
		pMesh->m_iNumFace = m_Parser.m_pAseMesh[dwObject]->m_iNumFace;		// 전체페이스 갯수					
		pMesh->m_strNodeName = m_Parser.m_pAseMesh[dwObject]->m_strNodeName;
		pMesh->m_strParentName = m_Parser.m_pAseMesh[dwObject]->m_strParentName;

		pMesh->m_matCalculation = m_Parser.m_pAseMesh[dwObject]->m_matWorld;
		pMesh->m_matWorldTrans = m_Parser.m_pAseMesh[dwObject]->m_matWorldTrans;
		pMesh->m_matWorldRotate = m_Parser.m_pAseMesh[dwObject]->m_matWorldRotate;
		pMesh->m_matWorldScale = m_Parser.m_pAseMesh[dwObject]->m_matWorldScale;

		D3DXMatrixInverse(&pMesh->m_matInverse, NULL, &pMesh->m_matWorld);

		int iRootMtrRef = m_Parser.m_pAseMesh[dwObject]->m_iMtrlRef;

		GMtrl* pMtrl = NULL;
		if (iRootMtrRef >= 0 && iRootMtrRef < m_Material.size())
		{
			pMtrl = &m_Material[iRootMtrRef];
			_ASSERT(pMtrl);
		}
		CStopwatch watch;
		if (pMesh->m_iNumFace > 0)
		{
			SetTriangleBuffer(pMesh.get(), m_Parser.m_pAseMesh[dwObject].get(), 0xff);

			if (pMtrl == NULL || pMtrl->m_SubMaterial.size() <= 0)
			{
				pMesh->SetUniqueBuffer(pMesh->m_TriList, -1, 0);
				pMesh->m_iTexIndex = -1;
				if (pMtrl)	pMesh->m_iTexIndex = GetMapID(pMtrl, ID_GBASIS_DI);
				m_iMaxVertex += pMesh->m_VertexArray.size();
				m_iMaxIndex += pMesh->m_IndexArray.size();
			}
			else
			{
				int iAddCount = 0;
				for (int iSub = 0; iSub < pMtrl->m_SubMaterial.size(); iSub++)
				{
					auto pSubMesh = make_shared<GMesh>();

					iAddCount += pSubMesh->SetUniqueBuffer(pMesh->m_TriList, iSub, iAddCount);

					GMtrl* pSubMtrl = &pMtrl->m_SubMaterial[iSub];
					_ASSERT(pSubMtrl);
					pSubMesh->m_iTexIndex = -1;
					if (pSubMtrl)
						pSubMesh->m_iTexIndex = GetMapID(pSubMtrl, ID_GBASIS_DI);

					m_iMaxVertex += pSubMesh->m_VertexArray.size();
					m_iMaxIndex += pSubMesh->m_IndexArray.size();

					if (pSubMesh->m_VertexArray.size() > 0)
					{
						pMesh->m_pSubMesh.push_back(pSubMesh);
					}
				}
			}
		}
		watch.Output(L"Sub Mesh");
		// 에니메이션 트랙 복사		
		GAnimTrack* pPrevTrack = NULL;
		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_PosTrack.size(); iTrack++)
		{
			auto pTrack = make_shared<GAnimTrack>();
			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_PosTrack[iTrack].iTick;
			pTrack->vVector = m_Parser.m_pAseMesh[dwObject]->m_PosTrack[iTrack].vVector;

			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
			pMesh->m_pPosTrack.push_back(pTrack);
		}

		pPrevTrack = NULL;
		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_RotTrack.size(); iTrack++)
		{
			auto pTrack = make_shared<GAnimTrack>();

			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].iTick;

			// 임의의 축과 각을 쿼터니언으로 변환			
			D3DXQuaternionRotationAxis(&pTrack->qRotate, &D3DXVECTOR3(m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.x,
				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.y,
				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.z),
				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.w);
			// 이전트랙의 쿼터니온과 누적시킴.			
			if (pPrevTrack != NULL)
			{
				D3DXQuaternionMultiply(&pTrack->qRotate, &pPrevTrack->qRotate, &pTrack->qRotate);
			}

			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);

			pMesh->m_pRotTrack.push_back(pTrack);
		}

		pPrevTrack = NULL;
		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_SclTrack.size(); iTrack++)
		{
			auto pTrack = make_shared<GAnimTrack>();

			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].iTick;
			pTrack->vVector = m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].vVector;

			// 임의의 축과 각을 쿼터니언으로 변환			
			D3DXQuaternionRotationAxis(&pTrack->qRotate,
				&D3DXVECTOR3(m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.x,
					m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.y,
					m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.z),
				m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.w);

			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);

			pMesh->m_pSclTrack.push_back(pTrack);
		}

		// 에니메이션 트랙 복사		
		pPrevTrack = NULL;
		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_VisTrack.size(); iTrack++)
		{
			auto pTrack = make_shared<GAnimTrack>();
			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_VisTrack[iTrack].iTick;
			pTrack->vVector = m_Parser.m_pAseMesh[dwObject]->m_VisTrack[iTrack].vVector;

			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
			pMesh->m_pVisTrack.push_back(pTrack);
		}
		m_pMesh.push_back(pMesh);
	}

	return InheriteCollect();
}
GAnimTrack* GAseObj::SetDoublyLinkedList(GAnimTrack* pCurrentTrack, GAnimTrack* pPrev)
{
	if (pPrev)
	{
		pPrev->pNext = pCurrentTrack;
		pCurrentTrack->pPrev = pPrev;
	}
	return pCurrentTrack;
}

void GAseObj::SetTriangleBuffer(GMesh* pMesh,
	GAseMesh* pAseMesh,
	DWORD dwMask)
{
	D3DXMATRIX matInverse;
	D3DXMatrixInverse(&matInverse, NULL, &pAseMesh->m_matWorld);

	pMesh->m_TriList.clear();
	pMesh->m_TriList.reserve(pAseMesh->m_iNumFace);
	pMesh->m_TriList.resize(pAseMesh->m_iNumFace);
	pMesh->m_iNumFace = pAseMesh->m_iNumFace;

	D3DXVECTOR3 vVertex;
	pMesh->m_BBox.vMin = D3DXVECTOR3(999999.0f, 999999.0f, 999999.0f);
	pMesh->m_BBox.vMax = D3DXVECTOR3(-999999.0f, -999999.0f, -999999.0f);

	int Index = 0;
	for (DWORD dwFace = 0; dwFace < pAseMesh->m_iNumFace; dwFace++)
	{
		DWORD dwMtrl = dwMask & pAseMesh->m_PosVertexList.pFaceList[dwFace].dwMtrl;
		pMesh->m_TriList[dwFace].iSubIndex = dwMtrl;
		if (pAseMesh->m_NorVertexList.pVertexList.size() > 0)
		{
			pMesh->m_TriList[dwFace].vNormal = pAseMesh->m_NorVertexList.pVertexList[dwFace * 4];
		}

		for (DWORD dwVertex = 0; dwVertex < 3; dwVertex++)
		{
			if (pAseMesh->m_PosVertexList.dwNumVertex >0 && pAseMesh->m_PosVertexList.dwNumFace >0)
			{
				pMesh->m_TriList[dwFace].vVertex[dwVertex].p =
					pAseMesh->m_PosVertexList.pVertexList[pAseMesh->m_PosVertexList.pFaceList[dwFace].v[dwVertex]];
			}

			if (pAseMesh->m_NorVertexList.pVertexList.size() > 0)
			{
				// 4 = FaceNormal[0], V0-Normal[1], V0-Normal[1], V0-Normal[2]
				pMesh->m_TriList[dwFace].vVertex[dwVertex].n = pAseMesh->m_NorVertexList.pVertexList[dwFace * 4 + dwVertex + 1];
			}

			if (pAseMesh->m_ColVertexList.dwNumVertex && pAseMesh->m_ColVertexList.dwNumFace)
			{
				// AseFile의 R G B => Load된 정보는 R B G로 저장되어 있다.
				pMesh->m_TriList[dwFace].vVertex[dwVertex].c.x = pAseMesh->m_ColVertexList.pVertexList[pAseMesh->m_ColVertexList.pFaceList[dwFace].v[dwVertex]].x;
				pMesh->m_TriList[dwFace].vVertex[dwVertex].c.y = pAseMesh->m_ColVertexList.pVertexList[pAseMesh->m_ColVertexList.pFaceList[dwFace].v[dwVertex]].z;
				pMesh->m_TriList[dwFace].vVertex[dwVertex].c.z = pAseMesh->m_ColVertexList.pVertexList[pAseMesh->m_ColVertexList.pFaceList[dwFace].v[dwVertex]].y;
				pMesh->m_TriList[dwFace].vVertex[dwVertex].c.w = 1.0f;
			}
			else
			{
				pMesh->m_TriList[dwFace].vVertex[dwVertex].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			if (pAseMesh->m_TexVertexList.dwNumVertex && pAseMesh->m_TexVertexList.dwNumFace)
			{
				float fV = pAseMesh->m_TexVertexList.pVertexList[pAseMesh->m_TexVertexList.pFaceList[dwFace].v[dwVertex]].x;
				//fV = fV * 2.0f;
				// AseFile의 U V W => Load된 정보는 U W V로 저장되어 있다.
				pMesh->m_TriList[dwFace].vVertex[dwVertex].t.x = fV;
				pMesh->m_TriList[dwFace].vVertex[dwVertex].t.y = 1.0f - pAseMesh->m_TexVertexList.pVertexList[pAseMesh->m_TexVertexList.pFaceList[dwFace].v[dwVertex]].z;
			}

			// 로칼 정점 = 월드 정점 * 월드행렬의 역행렬
			D3DXVec3TransformCoord(&pMesh->m_TriList[dwFace].vVertex[dwVertex].p,
				&pMesh->m_TriList[dwFace].vVertex[dwVertex].p,
				&matInverse);
			// 바운딩 박스 계산
			vVertex = pMesh->m_TriList[dwFace].vVertex[dwVertex].p;
			if (pMesh->m_BBox.vMin.x > vVertex.x) pMesh->m_BBox.vMin.x = vVertex.x;
			if (pMesh->m_BBox.vMin.y > vVertex.y) pMesh->m_BBox.vMin.y = vVertex.y;
			if (pMesh->m_BBox.vMin.z > vVertex.z) pMesh->m_BBox.vMin.z = vVertex.z;

			if (pMesh->m_BBox.vMax.x < vVertex.x) pMesh->m_BBox.vMax.x = vVertex.x;
			if (pMesh->m_BBox.vMax.y < vVertex.y) pMesh->m_BBox.vMax.y = vVertex.y;
			if (pMesh->m_BBox.vMax.z < vVertex.z) pMesh->m_BBox.vMax.z = vVertex.z;
		}
	}
	std::sort(pMesh->m_TriList.begin(),
		pMesh->m_TriList.end(),
		DescendingTriSort());
}
int	GAseObj::GetMapID(GMtrl* pMtrl, int iTexMapType)
{
	_ASSERT(pMtrl);
	if (pMtrl->m_TexMaps.size() <= 0) return -1;

	for (int iTexmap = 0; iTexmap < pMtrl->m_TexMaps.size(); iTexmap++)
	{
		if (pMtrl->m_TexMaps[iTexmap].m_dwType == iTexMapType)
		{
			return pMtrl->m_TexMaps[iTexmap].m_dwIndex;
		}
	}
	return -1;
}

HRESULT GAseObj::LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString)
{
	HRESULT hr = S_OK;
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
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
HRESULT GAseObj::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pShaderFile)
{
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_VertexArray.size() < 3) continue;
				pSubMesh->m_Object.g_pVertexShader.Attach(DX::LoadVertexShaderFile(pDevice, pShaderFile, pSubMesh->m_Object.g_pVSBlob.GetAddressOf()));
				pSubMesh->m_Object.g_pPixelShader.Attach(DX::LoadPixelShaderFile(pDevice, pShaderFile));
				pSubMesh->m_Object.g_pGeometryShader.Attach(DX::LoadGeometryShaderFile(pDevice, pShaderFile, pSubMesh->m_Object.g_pGSBlob.GetAddressOf()));
			}
		}
		else
		{
			if (pMesh->m_VertexArray.size() < 3) continue;
			pMesh->m_Object.g_pVertexShader.Attach(DX::LoadVertexShaderFile(pDevice, pShaderFile, pMesh->m_Object.g_pVSBlob.GetAddressOf()));
			pMesh->m_Object.g_pPixelShader.Attach(DX::LoadPixelShaderFile(pDevice, pShaderFile));
			pMesh->m_Object.g_pGeometryShader.Attach(DX::LoadGeometryShaderFile(pDevice, pShaderFile, pMesh->m_Object.g_pGSBlob.GetAddressOf()));
		}
	}

	return S_OK;
}
HRESULT GAseObj::CreateConstantBuffer()
{
	m_cbData.Color = D3DXVECTOR4(1, 1, 1, 1);
	//m_Object.g_pConstantBuffer.Attach(DX::CreateConstantBuffer(m_pd3dDevice, &m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_VertexArray.size() < 3) continue;
				pSubMesh->m_Object.g_pConstantBuffer.Attach(DX::CreateConstantBuffer(m_pd3dDevice, &pSubMesh->m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
			}
		}
		else
		{
			if (pMesh->m_VertexArray.size() < 3) continue;
			pMesh->m_Object.g_pConstantBuffer.Attach(DX::CreateConstantBuffer(m_pd3dDevice, &pMesh->m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
		}
	}
	return S_OK;
}
HRESULT GAseObj::SetInputLayout()
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

	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_VertexArray.size() < 3 ||
					pSubMesh->m_Object.g_pVSBlob == nullptr) continue;
				pSubMesh->m_Object.g_pInputlayout.Attach(DX::CreateInputlayout(m_pd3dDevice,
					pSubMesh->m_Object.g_pVSBlob.Get()->GetBufferSize(),
					pSubMesh->m_Object.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
			}
		}
		else
		{
			if (pMesh->m_VertexArray.size() < 3 ||
				pMesh->m_Object.g_pVSBlob == nullptr) continue;
			pMesh->m_Object.g_pInputlayout.Attach(DX::CreateInputlayout(m_pd3dDevice,
				pMesh->m_Object.g_pVSBlob.Get()->GetBufferSize(),
				pMesh->m_Object.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
		}
	}
	return hr;
}
HRESULT GAseObj::CreateVertexBuffer()
{
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_VertexArray.size() < 3) continue;
				pSubMesh->m_Object.g_pVertexBuffer.Attach(DX::CreateVertexBuffer(m_pd3dDevice,
					&pSubMesh->m_VertexArray.at(0),
					pSubMesh->m_VertexArray.size(),
					sizeof(PNCT_VERTEX)));
				pSubMesh->m_Object.m_iNumVertex = pSubMesh->m_VertexArray.size();
				pSubMesh->m_Object.m_iVertexSize = sizeof(PNCT_VERTEX);
			}
		}
		else
		{
			if (pMesh->m_VertexArray.size() < 3) continue;
			pMesh->m_Object.g_pVertexBuffer.Attach(DX::CreateVertexBuffer(m_pd3dDevice,
				&pMesh->m_VertexArray.at(0),
				pMesh->m_VertexArray.size(),
				sizeof(PNCT_VERTEX)));
			pMesh->m_Object.m_iNumVertex = pMesh->m_VertexArray.size();
			pMesh->m_Object.m_iVertexSize = sizeof(PNCT_VERTEX);
		}
	}
	return S_OK;
}
HRESULT	 GAseObj::CreateIndexBuffer()
{
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		GMesh* pMesh = m_pMesh[dwObject].get();
		if (pMesh->m_pSubMesh.size() > 0)
		{
			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
			{
				GMesh* pSubMesh = pMesh->m_pSubMesh[dwSub].get();
				if (pSubMesh->m_VertexArray.size() < 3) continue;
				pSubMesh->m_Object.g_pIndexBuffer.Attach(DX::CreateIndexBuffer(m_pd3dDevice,
					&pSubMesh->m_IndexArray.at(0),
					pSubMesh->m_IndexArray.size(),
					sizeof(DWORD)));
				pSubMesh->m_Object.m_iNumIndex = pSubMesh->m_IndexArray.size();

			}
		}
		else
		{
			if (pMesh->m_VertexArray.size() < 3) continue;
			pMesh->m_Object.g_pIndexBuffer.Attach(DX::CreateIndexBuffer(g_pd3dDevice,
				&pMesh->m_IndexArray.at(0),
				pMesh->m_IndexArray.size(),
				sizeof(DWORD)));
			pMesh->m_Object.m_iNumIndex = pMesh->m_IndexArray.size();
		}
	}
	return S_OK;
}
bool GAseObj::ResetResource()
{
	return true;
}

bool GAseObj::InheriteCollect()
{
	D3DXMATRIX m_matInverse;
	D3DXQUATERNION qR;
	D3DXVECTOR3 vTrans, vScale;
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		if (!m_pMesh[dwObject]->m_strParentName.empty())
		{
			GMesh* pParentNode = SearchToCollects(m_pMesh[dwObject]->m_strParentName);
			if (pParentNode)
			{
				m_pMesh[dwObject]->m_pParent = pParentNode;

				m_matInverse = m_pMesh[dwObject]->m_matWorld * pParentNode->m_matInverse;
				D3DXMatrixDecompose(&vScale, &qR, &vTrans, &m_matInverse);
				D3DXMatrixScaling(&m_pMesh[dwObject]->m_matWorldScale, vScale.x, vScale.y, vScale.z);
				D3DXMatrixTranslation(&m_pMesh[dwObject]->m_matWorldTrans, vTrans.x, vTrans.y, vTrans.z);
				D3DXMatrixRotationQuaternion(&m_pMesh[dwObject]->m_matWorldRotate, &qR);

				pParentNode->m_pChildMesh.push_back(m_pMesh[dwObject].get());
			}
		}
	}
	// 사용되지 않은 오브젝트 삭제
	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
	{
		if (m_pMesh[dwObject]->m_pChildMesh.size() <= 0 &&
			(m_pMesh[dwObject]->m_ClassType == CLASS_BONE || m_pMesh[dwObject]->m_ClassType == CLASS_DUMMY))
		{
			m_pMesh[dwObject]->m_bUsed = false;
		}
	}
	return true;
}
GMesh* GAseObj::SearchToCollects(T_STR	m_strParentName)
{
	for (DWORD dwGroup = 0; dwGroup < m_pMesh.size(); dwGroup++)
	{
		if (m_pMesh[dwGroup]->m_strNodeName == m_strParentName)
		{
			return m_pMesh[dwGroup].get();
		}
	}
	return NULL;
}
GAseObj::GAseObj(void)
{
}

GAseObj::~GAseObj(void)
{
}
