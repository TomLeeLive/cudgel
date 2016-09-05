#include "_stdafx.h"

//스트링 편집하여 텍스처 파일 경로를 실제 경로로 맞춰준다.
void GAseParser::GetStringWeNeed(VOID* pOutStr, VOID* pInStr) {

	vector<TCHAR*> vString;

	TCHAR* token =NULL;
	token = _tcstok((TCHAR*)pInStr, L"\\");
	while (token != NULL)
	{
		token = _tcstok(NULL, L"\\");
		vString.push_back(token);
	}
	_tcscpy((TCHAR*)pOutStr, vString[vString.size() - 2]);

	TCHAR strDir[MAX_PATH] = L"data\\";
	_tcsncat(strDir, (TCHAR*)pOutStr, _tcsclen((TCHAR*)pOutStr));

	_tcscpy((TCHAR*)pOutStr, strDir);

}




int		GAseParser::GetMeshDataFromFile(GAseModel* stModel) {


	int iSize = sizeof(g_pAseMeshTokens) / sizeof(g_pAseMeshTokens[0]);

	while (!feof( m_pStream))
	{
		_fgetts( m_pBuffer, 256,  m_pStream);
		_stscanf( m_pBuffer, _T("%s"),  m_pString);


		for (int i = 0; i < iSize; i++) {


			if (!_tcsicmp( m_pString, g_pAseMeshTokens[i]))
			{
				switch (i)
				{

				case MESH_NUMVERTEX:
				{
					GetData(&(stModel->m_vObj[0]->m_iPosCount), INT_DATA);
					break;
				}
				break;
				case MESH_NUMFACES:
				{
					GetData(&(stModel->m_vObj[0]->m_iFaceCount), INT_DATA);
				}
				break;
				case MESH_VERTEX_LIST:
				{
					D3DXVECTOR3 pPos;
					for (int i = 0; i < stModel->m_vObj[0]->m_iPosCount; i++) {
						 GetDataFromFileLoop(L"*MESH_VERTEX", &pPos, MESH_VERTEX_DATA);
						 stModel->m_vObj[0]->m_vPosList.push_back(pPos);
					}
				}
				break;
				case MESH_FACE_LIST:
				{
					ST_INT_FOUR stMeshFace;

					for (int i = 0; i < stModel->m_vObj[0]->m_iFaceCount; i++) {
						GetDataFromFileLoop(L"*MESH_FACE", &stMeshFace, MESH_FACE_DATA);
						stModel->m_vObj[0]->m_vIndex.push_back(stMeshFace.index1);
						stModel->m_vObj[0]->m_vIndex.push_back(stMeshFace.index2);
						stModel->m_vObj[0]->m_vIndex.push_back(stMeshFace.index3);

						if (stModel->m_vMaterial[0]->m_vSubMaterial.size() != 0)
							stModel->m_vObj[0]->m_vSubMtlIndex.push_back(stMeshFace.index4);
					}


				}
				break;
				case MESH_NUMTVERTEX:
				{
					 GetData(&(stModel->m_vObj[0]->m_iTexVerCount), INT_DATA);
				}
				break;
				case MESH_TVERTLIST:
				{
					D3DXVECTOR3 vUVData; float fTemp;
					for (int i = 0; i < stModel->m_vObj[0]->m_iTexVerCount; i++) {
						 GetDataFromFileLoop(L"*MESH_TVERT", &vUVData, MESH_VERTEX_DATA);

						fTemp = vUVData.z;
						vUVData.z = vUVData.y;
						vUVData.y = fTemp;

						vUVData.y = 1.0f - vUVData.y;

						stModel->m_vObj[0]->m_vTexList.push_back(vUVData);
					}
				}
				break;
				case MESH_NUMTVFACES:
				{
					 GetData(&(stModel->m_vObj[0]->m_iTexFaceCount), INT_DATA);
				}
				break;
				case MESH_TFACELIST:
				{
					ST_INT_THREE stFaceList;
					for (int i = 0; i < stModel->m_vObj[0]->m_iTexFaceCount; i++) {
						 GetDataFromFileLoop(L"*MESH_TFACE", &stFaceList, FACE_DATA);
						 stModel->m_vObj[0]->m_vTextureIndex.push_back(stFaceList.index1);
						 stModel->m_vObj[0]->m_vTextureIndex.push_back(stFaceList.index2);
						 stModel->m_vObj[0]->m_vTextureIndex.push_back(stFaceList.index3);
					}
				}
				break;
				case MESH_NUMCVERTEX:
				{
					 GetData(&(stModel->m_vObj[0]->m_iColorVerCount), INT_DATA);
				}
				break;
				case MESH_CVERTLIST:
				{
					D3DXVECTOR3 vColor;
					float fTemp;

					for (int i = 0; i < stModel->m_vObj[0]->m_iColorVerCount; i++) {
						 GetDataFromFileLoop(L"*MESH_VERTCOL", &vColor, MESH_VERTEX_DATA);
						 
						 fTemp = vColor.y;
						 vColor.y = vColor.z;
						 vColor.z = fTemp;

						 stModel->m_vObj[0]->m_vColList.push_back(vColor);
					}
				}
				break;
				case MESH_NORMALS:
				{
					D3DXVECTOR3 vNormal;
					for (int i = 0; i < stModel->m_vObj[0]->m_iTexFaceCount; i++) {
						 GetDataFromFileLoop(L"*MESH_FACENORMAL", &vNormal, MESH_VERTEX_DATA);
						 stModel->m_vObj[0]->m_vFaceNormal.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						 stModel->m_vObj[0]->m_vNorList.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						 stModel->m_vObj[0]->m_vNorList.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						 stModel->m_vObj[0]->m_vNorList.push_back(vNormal);
					}
					return 0;
				}
				break;

				}
			}
		}
		//fgetc( m_pStream);
	}

	return 0;
}


bool GAseParser::GetTrackListFromString(vector<GAnimTrack>& vTrack,
	AseTrackType TrackType)
{
	GAnimTrack Track;

	while (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);



		if (!_tcsicmp(m_pString, L"}"))
			break;

		if (TrackType == POS_SAMPLE_TRACK)
		{
			ST_ANI_POS stPosData;
			//GetDataFromFileLoop(L"*CONTROL_POS_SAMPLE", &stPosData, ANI_POS_DATA);
			GetData(&stPosData, ANI_POS_DATA);
			Track.vecVector = stPosData.vecPos;
			Track.iTick = stPosData.iTick;
			vTrack.push_back(Track);
			//_stscanf(GetNextTokenString(), _T("%s%d%f%f%f"), m_pString, &Track.iTick,
			//	&Track.vVector.x,
			//	&Track.vVector.z,
			//	&Track.vVector.y);
		}
		else if (TrackType == ROT_SAMPLE_TRACK)
		{
			ST_ANI_ROT stRotData;
			GetData(&stRotData, ANI_ROT_DATA);
			Track.qRotate.x = stRotData.vecRot.x;
			Track.qRotate.y = stRotData.vecRot.y;
			Track.qRotate.z = stRotData.vecRot.z;
			Track.qRotate.w = stRotData.vecRot.w;
			Track.iTick = stRotData.iTick;
			vTrack.push_back(Track);

			//_stscanf(GetNextTokenString(), _T("%s%d%f%f%f%f"), m_pString, &Track.iTick,
			//	&Track.qRotate.x, &Track.qRotate.z, &Track.qRotate.y, &Track.qRotate.w);
		}
		else if (TrackType == SCL_SAMPLE_TRACK)
		{
			ST_ANI_SCL stSclData;
			GetData(&stSclData, ANI_SCL_DATA);
			Track.vecVector = stSclData.vecVec;
			Track.qRotate.x = stSclData.vecRot.x;
			Track.qRotate.y = stSclData.vecRot.y;
			Track.qRotate.z = stSclData.vecRot.z;
			Track.qRotate.w = stSclData.vecRot.w;
			Track.iTick = stSclData.iTick;
			vTrack.push_back(Track);

			//_stscanf(GetNextTokenString(), _T("%s%d%f%f%f %f%f%f%f"), m_pString, &Track.iTick,
			//	&Track.vVector.x, &Track.vVector.z, &Track.vVector.y,
			//	&Track.qRotate.x, &Track.qRotate.z, &Track.qRotate.y, &Track.qRotate.w);
		}
		else if (TrackType == VIS_SAMPLE_TRACK)
		{
			//_stscanf(GetNextTokenString(), _T("%s%d%f"),
			//	m_pString, &Track.iTick,
			//	&Track.vVector.x);
		}
		/*vTrack.push_back(Track);*/

		//if (_tcsstr(m_pwcTokenData.c_str(), _T("}")) != NULL)
		//{
		//	break;
		//}
	}
	return true;
}

int		GAseParser::GetAnimationDataFromFile(GAseModel* stModel) {


	int iSize = sizeof(g_pAseAniDataTokens) / sizeof(g_pAseAniDataTokens[0]);

	while (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);

		for (int i = 0; i < iSize; i++) {


			if (!_tcsicmp(m_pString, g_pAseAniDataTokens[i]))
			{
				switch (i)
				{
				case ANI_CONTROL_POS_TCB:
				case ANI_CONTROL_POS_BEZIER:
				case ANI_CONTROL_POS_TRACK:
				{
					GetTrackListFromString(stModel->m_vObj[0].get()->m_vPosTrack, POS_SAMPLE_TRACK);
				}
				break;
				case ANI_CONTROL_ROT_TCB:
				case ANI_CONTROL_ROT_BEZIER:
				case ANI_CONTROL_ROT_TRACK:
				{
					GetTrackListFromString(stModel->m_vObj[0].get()->m_vRotTrack, ROT_SAMPLE_TRACK);
				}
				break;
				case ANI_CONTROL_SCALE_TCB:
				case ANI_CONTROL_SCALE_BEZIER:
				case ANI_CONTROL_SCALE_TRACK:
				{
					GetTrackListFromString(stModel->m_vObj[0].get()->m_vSclTrack, SCL_SAMPLE_TRACK);
				}
				break;

				}
			}
		}
		//fgetc( m_pStream);
	}


	return 0;
}
int		GAseParser::GetObjDataFromFile(GAseModel* stModel) {

	//GAseObj* aseobj = new GAseObj;

	if (stModel->m_vMaterial[0]->m_vSubMaterial.size() == 0) {
		stModel->m_vObj.push_back(make_shared<GAseObj>());
	}
	else {
		for (int i = 0; i< stModel->m_vMaterial[0]->m_vSubMaterial.size(); i++)   //0번 은 오리지널 데이터로 남겨놓는다.
			stModel->m_vObj.push_back(make_shared<GAseObj>());
	}


	int iSize = sizeof(g_pAseObjTokens) / sizeof(g_pAseObjTokens[0]);

	while (!feof( m_pStream))
	{
		_fgetts( m_pBuffer, 256,  m_pStream);
		_stscanf( m_pBuffer, _T("%s"),  m_pString);


		for (int i = 0; i < iSize; i++) {

			//if (i == ANIMATION) {
			//	SaveFilePosition()
			//}

			if (!_tcsicmp( m_pString, g_pAseObjTokens[i]))
			{
				switch (i)
				{

				case NODE_NAME:
				{

					GetData(&(stModel->m_vObj[0]->m_szName), STRING_DATA);

				}
				break;
				case NODE_TM:
				{
					D3DXVECTOR3 vecROW0;
					D3DXVECTOR3 vecROW1;
					D3DXVECTOR3 vecROW2;
					D3DXVECTOR3 vecROW3;

					GetDataFromFileLoop(g_pAseNodeTmTokens[0], &vecROW0, VERTEX_DATA);
					GetDataFromFileLoop(g_pAseNodeTmTokens[1], &vecROW1, VERTEX_DATA);
					GetDataFromFileLoop(g_pAseNodeTmTokens[2], &vecROW2, VERTEX_DATA);
					GetDataFromFileLoop(g_pAseNodeTmTokens[3], &vecROW3, VERTEX_DATA);

					GetData(&(stModel->m_vObj[0]->m_szName), STRING_DATA);

					D3DXMatrixIdentity(&(stModel->m_vObj[0]->m_matWorld));

					stModel->m_vObj[0]->m_matWorld._11 = vecROW0.x; stModel->m_vObj[0]->m_matWorld._12 = vecROW0.y; stModel->m_vObj[0]->m_matWorld._13 = vecROW0.z;
					stModel->m_vObj[0]->m_matWorld._31 = vecROW1.x; stModel->m_vObj[0]->m_matWorld._32 = vecROW1.y; stModel->m_vObj[0]->m_matWorld._33 = vecROW1.z;
					stModel->m_vObj[0]->m_matWorld._21 = vecROW2.x; stModel->m_vObj[0]->m_matWorld._22 = vecROW2.y; stModel->m_vObj[0]->m_matWorld._23 = vecROW2.z;
					stModel->m_vObj[0]->m_matWorld._41 = vecROW3.x; stModel->m_vObj[0]->m_matWorld._42 = vecROW3.y; stModel->m_vObj[0]->m_matWorld._43 = vecROW3.z;
				}
				break;
				case MESH:
				{
					GetMeshDataFromFile(stModel);
				}
				break;

				case ANIMATION:
				{
					stModel->m_vObj[0]->m_bHasAniTrack = true;
					GetAnimationDataFromFile(stModel);
				}
				break;

				}
			}
		}
		//fgetc( m_pStream);
	}


	return 0;

}
int		GAseParser::GetDataFromFile(GAseModel* stModel ){

	int iSize = sizeof(g_pAseItems) / sizeof(g_pAseItems[0]);

	while (!feof( m_pStream))
	{
		_fgetts( m_pBuffer, 256,  m_pStream);
		_stscanf( m_pBuffer, _T("%s"),  m_pString);


		for (int i = 0; i < iSize; i++) {

			if (!_tcsicmp( m_pString, g_pAseItems[i]))
			{
				switch (i)
				{
				case SCENE:
				{
					 GetDataFromFileLoop(g_pAseSceneTokens[0], &stModel->m_stScene.m_szName, STRING_DATA);
					 GetDataFromFileLoop(g_pAseSceneTokens[1], &stModel->m_stScene.m_iFrame, INT_DATA);
					 GetDataFromFileLoop(g_pAseSceneTokens[2], &stModel->m_stScene.m_iLastFrame, INT_DATA);
					 GetDataFromFileLoop(g_pAseSceneTokens[3], &stModel->m_stScene.m_iFrameSpeed, INT_DATA);
					 GetDataFromFileLoop(g_pAseSceneTokens[4], &stModel->m_stScene.m_iTicksPerFrame, INT_DATA);
				}
				break;
				case MATERIALLIST:
				{
					int iSubMtls = 0;

					auto material = make_shared<GAseMaterial>();
					GetDataFromFileLoop(g_pAseMaterialTokens[0], &(material.get()->m_szName), STRING_DATA);
					GetDataFromFileLoop(g_pAseMaterialTokens[1], &(material.get()->m_vecAmbient), VERTEX_DATA);
					GetDataFromFileLoop(g_pAseMaterialTokens[2], &(material.get()->m_vecDiffuse), VERTEX_DATA);
					GetDataFromFileLoop(g_pAseMaterialTokens[3], &(material.get()->m_vecSpecular), VERTEX_DATA);


					SaveFilePosition();

					GetDataFromFileLoop(L"*NUMSUBMTLS", &iSubMtls, INT_DATA);

					if (iSubMtls == 0) {
						RestoreFilePosition();
						GetDataFromFileLoop(g_pAseMaterialTokens[4], &(material.get()->m_szMapDiffuse), STRING_DATA);
						GetStringWeNeed(material.get()->m_szMapDiffuse, material.get()->m_szMapDiffuse);
						//material->m_iSubMaterial = 0;
					}
					else {
						memset(&(material.get()->m_szMapDiffuse), 0, sizeof(&(material.get()->m_szMapDiffuse)));
						//material->m_iSubMaterial = iSubMtls;

						for (int i = 0; i < iSubMtls; i++){
							auto submaterial = make_shared<GAseMaterial>();
							GetDataFromFileLoop(g_pAseMaterialTokens[0], &(submaterial.get()->m_szName), STRING_DATA);
							GetDataFromFileLoop(g_pAseMaterialTokens[1], &(submaterial.get()->m_vecAmbient), VERTEX_DATA);
							GetDataFromFileLoop(g_pAseMaterialTokens[2], &(submaterial.get()->m_vecDiffuse), VERTEX_DATA);
							GetDataFromFileLoop(g_pAseMaterialTokens[3], &(submaterial.get()->m_vecSpecular), VERTEX_DATA);
							GetDataFromFileLoop(g_pAseMaterialTokens[4], &(submaterial.get()->m_szMapDiffuse), STRING_DATA);
							
							GetStringWeNeed(submaterial.get()->m_szMapDiffuse, submaterial.get()->m_szMapDiffuse);
							material->m_vSubMaterial.push_back(submaterial);
						}
					}

					stModel->m_vMaterial.push_back(material);
				}
				break;
				case GEOMOBJECT:
				{
					GetObjDataFromFile(stModel);
				}
				break;
				}
			}
		}
		//fgetc( m_pStream);
	}



	return 0;
}


void    GAseParser::SetPnctData(GAseModel* stModel) {

	D3DXVECTOR3		vp;
	D3DXVECTOR3		vn;
	D3DXVECTOR4		vc;
	D3DXVECTOR2     vt;
	D3DXMATRIX		matWorldInverse;


	// Create index buffer
	WORD* indices;
	indices = (WORD *)malloc(sizeof(WORD) * stModel->m_vObj[0]->m_iFaceCount * 3);


	for (int i = 0; i < stModel->m_vObj[0]->m_iFaceCount * 3; i++) {

		if (i == 0 || i % 3 == 0) {
			indices[i] = { (WORD)(stModel->m_vObj[0]->m_vIndex[i]) };
		}
		else if (i == 1 || i % 3 == 1) {
			indices[i] = { (WORD)(stModel->m_vObj[0]->m_vIndex[i + 1]) };
		}
		else if (i == 2 || i % 3 == 2) {
			indices[i] = { (WORD)(stModel->m_vObj[0]->m_vIndex[i - 1]) };
		}
	}


	WORD* Texindices;
	Texindices = (WORD *)malloc(sizeof(WORD) * stModel->m_vObj[0]->m_iFaceCount * 3);


	for (int i = 0; i < stModel->m_vObj[0]->m_iFaceCount * 3; i++) {

		if (i == 0 || i % 3 == 0) {
			Texindices[i] = { (WORD)(stModel->m_vObj[0]->m_vTextureIndex[i]) };
		}
		else if (i == 1 || i % 3 == 1) {
			Texindices[i] = { (WORD)(stModel->m_vObj[0]->m_vTextureIndex[i + 1]) };
		}
		else if (i == 2 || i % 3 == 2) {
			Texindices[i] = { (WORD)(stModel->m_vObj[0]->m_vTextureIndex[i - 1]) };
		}
	}


	D3DXMatrixInverse(&matWorldInverse, NULL, &stModel->m_vObj[0]->m_matWorld);






		for (int i = 0; i < stModel->m_vObj[0]->m_iFaceCount * 3; i++) {

			vp = stModel->m_vObj[0]->m_vPosList[indices[i]];

			D3DXVec3TransformCoord(&vp, &vp, &matWorldInverse);

			vn = stModel->m_vObj[0]->m_vNorList[indices[i]];

			if (stModel->m_vObj[0]->m_iColorVerCount != 0)
				vc = D3DXVECTOR4(stModel->m_vObj[0]->m_vColList[indices[i]].x, stModel->m_vObj[0]->m_vColList[indices[i]].y, stModel->m_vObj[0]->m_vColList[indices[i]].z, 1.0f);
			else
				vc = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

			vt = D3DXVECTOR2(stModel->m_vObj[0]->m_vTexList[Texindices[i]].x, stModel->m_vObj[0]->m_vTexList[Texindices[i]].y);

			if (stModel->m_vMaterial[0]->m_vSubMaterial.size() == 0) {
				stModel->m_vObj[0]->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
			}
			else {
				int j = i / 3;
				stModel->m_vObj[    stModel->m_vObj[0]->m_vSubMtlIndex[j]    ]->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
			}
		}



	delete[] Texindices;
	delete[] indices;

}

GAseParser::GAseParser()
{
}


GAseParser::~GAseParser()
{
}
