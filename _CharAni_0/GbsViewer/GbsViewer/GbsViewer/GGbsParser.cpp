#include "_stdafx.h"

//To-Do:상속관계구축
void GGbsParser::ProcessInheritanceBtwObjs(GGbsModel* stModel) {


	TCHAR	pNodeName[256];

	TCHAR	pParentNodeName[256];

	GGbsGeom* pChildObj;
	GGbsGeom* pParentObj;

	memset(&pNodeName, 0, sizeof(pNodeName));
	memset(&pParentNodeName, 0, sizeof(pParentNodeName));

	while (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);

		if (!_tcsicmp(m_pString, L"*NODE_NAME"))
		{
			GetData(&pNodeName, STRING_DATA);

			_fgetts(m_pBuffer, 256, m_pStream);
			_stscanf(m_pBuffer, _T("%s"), m_pString);

			if (!_tcsicmp(m_pString, L"*NODE_PARENT"))
			{

				GetData(&pParentNodeName, STRING_DATA);

				//찾는다. 상속관계 저장.
				//1) L"*NODE_NAME"
				//2) L"*NODE_PARENT"

				for (int i = 0; i < stModel->m_vGeomObj.size(); i++) {
					if (!_tcsicmp(stModel->m_vGeomObj[i]->m_szName, pNodeName))
					{
						pChildObj = stModel->m_vGeomObj[i].get();
						break;
					}

				}

				for (int i = 0; i < stModel->m_vGeomObj.size(); i++) {
					if (!_tcsicmp(stModel->m_vGeomObj[i]->m_szName, pParentNodeName))
					{
						pParentObj = stModel->m_vGeomObj[i].get();
						break;
					}
					
				}
				pParentObj->m_pChildObj.push_back(pChildObj);
				pChildObj->m_pParentObj = pParentObj;

				memset(&pNodeName, 0, sizeof(pNodeName));
				memset(&pParentNodeName, 0, sizeof(pParentNodeName));
			}

		}
	}
	for (int i = 0; i < stModel->m_vGeomObj.size(); i++) {
		if (stModel->m_vGeomObj[i]->m_pParentObj == NULL && stModel->m_vGeomObj[i]->m_pChildObj.size() == 0)
			stModel->m_vGeomObj[i]->m_bUsed = false;

		if (stModel->m_vGeomObj[i]->m_pParentObj != NULL) {

			D3DXMATRIX matTranslate, matRotation, matScale;
			D3DXQUATERNION qR;
			D3DXVECTOR3 vTrans, vScale;

			D3DXMATRIX InversepWM;
			D3DXMatrixInverse(&InversepWM, NULL, &stModel->m_vGeomObj[i]->m_pParentObj->m_matWld);
			stModel->m_vGeomObj[i]->m_matChlWld = stModel->m_vGeomObj[i]->m_matWld * InversepWM;

			// 행렬을 분해    
			D3DXMatrixDecompose(&vScale, &qR, &vTrans, &stModel->m_vGeomObj[i]->m_matChlWld);

			D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
			D3DXMatrixTranslation(&matTranslate, vTrans.x, vTrans.y, vTrans.z);
			D3DXMatrixRotationQuaternion(&matRotation, &qR);

			stModel->m_vGeomObj[i]->m_matWldTrans = matTranslate;
			stModel->m_vGeomObj[i]->m_matWldRotate = matRotation;
			stModel->m_vGeomObj[i]->m_matWldScale = matScale;
			//stModel->m_vGeomObj[i]->m_qRotation = qR;

		}
	}



	
}
void GGbsParser::CountGeomObjFromFile(GGbsModel* stModel) {

	m_iObjCount = 0;
	//To-Do: 현재 읽은 ASE 파일이 Multi Object Animation인지 판정하여 
	//vector<shared_ptr<GAseGeom>>			m_vGeomObj; 에 GAseGeom 객체를 생성하여 push 한다.(Geomobject 갯수 만큼)
	while (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);

		if (!_tcsicmp(m_pString, L"#OBJECT"))
		{
			auto gbsgeom = make_shared<GGbsGeom>();
			gbsgeom->m_iType = 0;
			stModel->m_vGeomObj.push_back(gbsgeom);
		}
		//else if (!_tcsicmp(m_pString, L"*HELPEROBJECT"))
		//{
		//	auto asegeom = make_shared<GGbsGeom>();
		//	asegeom->m_iType = 1;
		//	stModel->m_vGeomObj.push_back(asegeom);
		//}
	}
}

void	GGbsParser::InitGbsModel(TCHAR* strFile, GGbsModel* stModel) {
	
	//파일 읽어서 필요한 정보 가져온다.
	//*GEOMOBJECT 과 *HELPEROBJECT를 카운트하여 객체생성후 stModel->m_vGeomObj에 push 한다.
	//OpenStream(strFile);
	//CountGeomObjFromFile(stModel);
	//CloseStream();

	
	
	//파일 읽어서 필요한 정보 가져온다.
	//To-Do:헬퍼 오브젝트등.. 파싱
	/*  *GEOMOBJECT *HELPEROBJECT *TM_ANIMATION *BOUNDINGBOX_MIN *BOUNDINGBOX_MAX  */
	OpenStream(strFile);
	GetDataFromFile(stModel);
	CloseStream();


	/*
	
	if(stModel->m_vGeomObj.size() == 1) //멀티 오브젝트가 아닐때...
		SetPnctData(stModel);
	
	*/

	//파일 읽어서 필요한 정보 가져온다.
	//To-Do:상속관계구축
	if (stModel->m_vGeomObj.size() > 1){
		OpenStream(strFile);
		ProcessInheritanceBtwObjs(stModel);
		CloseStream();
	}


	//To-Do:NODE_TM 행렬처리(부모행렬과 결합된 것 부모행렬의 역행렬을 곱해줌)

	//To-Do: 불필요한헬퍼오브젝트 렌더에서 제외


}
//스트링 편집하여 텍스처 파일 경로를 실제 경로로 맞춰준다.
void GGbsParser::GetStringWeNeed(VOID* pOutStr, VOID* pInStr) {

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




int		GGbsParser::GetMeshDataFromFile(GGbsModel* stModel) {

	//int iFaceCountSubtract = 0;

	int iSize = sizeof(g_pGbsMeshTokens) / sizeof(g_pGbsMeshTokens[0]);

	while (!feof( m_pStream))
	{
		SaveFilePosition();
		if (!_tcsicmp(m_pString, L"}")) {

			_fgetts(m_pBuffer, 256, m_pStream);
			_stscanf(m_pBuffer, _T("%s"), m_pString);

			if (!_tcsicmp(m_pString, L"}")) {
				RestoreFilePosition();
				break;
			}
			else {
				RestoreFilePosition();
			}
		}

		_fgetts( m_pBuffer, 256,  m_pStream);
		_stscanf( m_pBuffer, _T("%s"),  m_pString);



		for (int i = 0; i < iSize; i++) {


			if (!_tcsicmp( m_pString, g_pGbsMeshTokens[i]))
			{
				switch (i)
				{

				case MESH_NUMVERTEX:
				{
					GetData(&m_iPosCount, INT_DATA);
					break;
				}
				break;
				case MESH_NUMFACES:
				{
					GetData(&m_iFaceCount, INT_DATA);
				}
				break;
				case MESH_VERTEX_LIST:
				{
					D3DXVECTOR3 pPos;
					for (int i = 0; i < m_iPosCount; i++) {
						 GetDataFromFileLoop(L"*MESH_VERTEX", &pPos, MESH_VERTEX_DATA);
						 m_vPosList.push_back(pPos);
					}
				}
				break;
				case MESH_FACE_LIST:
				{
					ST_INT_FOUR stMeshFace;

					for (int i = 0; i < m_iFaceCount; i++) {
						GetDataFromFileLoop(L"*MESH_FACE", &stMeshFace, MESH_FACE_DATA);

						//if (stMeshFace.index4 == 255){
						//	iFaceCountSubtract++;
						//	continue;
						//}

						m_vIndex.push_back(stMeshFace.index1);
						m_vIndex.push_back(stMeshFace.index2);
						m_vIndex.push_back(stMeshFace.index3);

						if (stModel->m_vMaterial[0]->m_vSubMaterial.size() != 0)
							m_vSubMtlIndex.push_back(stMeshFace.index4);
					}


				}
				break;
				case MESH_NUMTVERTEX:
				{
					 GetData(&m_iTexVerCount, INT_DATA);
				}
				break;
				case MESH_TVERTLIST:
				{
					D3DXVECTOR3 vUVData; float fTemp;
					for (int i = 0; i < m_iTexVerCount; i++) {
						 GetDataFromFileLoop(L"*MESH_TVERT", &vUVData, MESH_VERTEX_DATA);

						fTemp = vUVData.z;
						vUVData.z = vUVData.y;
						vUVData.y = fTemp;

						vUVData.y = 1.0f - vUVData.y;

						m_vTexList.push_back(vUVData);
					}
				}
				break;
				case MESH_NUMTVFACES:
				{
					 GetData(&m_iTexFaceCount, INT_DATA);
				}
				break;
				case MESH_TFACELIST:
				{
					ST_INT_THREE stFaceList;
					for (int i = 0; i < m_iTexFaceCount; i++) {
						 GetDataFromFileLoop(L"*MESH_TFACE", &stFaceList, FACE_DATA);
						 m_vTextureIndex.push_back(stFaceList.index1);
						 m_vTextureIndex.push_back(stFaceList.index2);
						 m_vTextureIndex.push_back(stFaceList.index3);
					}
				}
				break;
				case MESH_NUMCVERTEX:
				{
					 GetData(&m_iColorVerCount, INT_DATA);
				}
				break;
				case MESH_CVERTLIST:
				{
					D3DXVECTOR3 vColor;
					float fTemp;

					for (int i = 0; i < m_iColorVerCount; i++) {
						 GetDataFromFileLoop(L"*MESH_VERTCOL", &vColor, MESH_VERTEX_DATA);
						 
						 fTemp = vColor.y;
						 vColor.y = vColor.z;
						 vColor.z = fTemp;

						 m_vColList.push_back(vColor);
					}
				}
				break;
				case MESH_NORMALS:
				{
					D3DXVECTOR3 vNormal;
					for (int i = 0; i < m_iTexFaceCount; i++) {
						 GetDataFromFileLoop(L"*MESH_FACENORMAL", &vNormal, MESH_VERTEX_DATA);
						 m_vFaceNormal.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						 m_vNorList.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						 m_vNorList.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						 m_vNorList.push_back(vNormal);
					}

					//m_iFaceCount -= iFaceCountSubtract;

					//multiobj일 경우 ... SetPnctData를 호출해서 해당 obj의 pnct vector에 push back을 한다.
					if (stModel->m_vGeomObj.size() > 1) //멀티 오브젝트일 경우..
					{
						SetPnctData(stModel,m_iObjCount);
						ResetVariables();
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
/*
void GAseParser::SetPnctMultiObjData(GAseModel* stModel, int iObjNum) {
	D3DXVECTOR3		vp;
	D3DXVECTOR3		vn;
	D3DXVECTOR4		vc;
	D3DXVECTOR2     vt;
	D3DXMATRIX		matWorldInverse;


	// Create index buffer
	WORD* indices;
	indices = (WORD *)malloc(sizeof(WORD) * m_iFaceCount * 3);

	if (m_vIndex.size() != 0) {
		for (int i = 0; i < m_iFaceCount * 3; i++) {

			if (i == 0 || i % 3 == 0) {
				indices[i] = { (WORD)(m_vIndex[i]) };
			}
			else if (i == 1 || i % 3 == 1) {
				indices[i] = { (WORD)(m_vIndex[i + 1]) };
			}
			else if (i == 2 || i % 3 == 2) {
				indices[i] = { (WORD)(m_vIndex[i - 1]) };
			}
		}
	}


	WORD* Texindices;
	Texindices = (WORD *)malloc(sizeof(WORD) * m_iFaceCount * 3);


	if (m_vTextureIndex.size() != 0) {
		for (int i = 0; i < m_iFaceCount * 3; i++) {

			if (i == 0 || i % 3 == 0) {
				Texindices[i] = { (WORD)(m_vTextureIndex[i]) };
			}
			else if (i == 1 || i % 3 == 1) {
				Texindices[i] = { (WORD)(m_vTextureIndex[i + 1]) };
			}
			else if (i == 2 || i % 3 == 2) {
				Texindices[i] = { (WORD)(m_vTextureIndex[i - 1]) };
			}
		}
	}

	D3DXMatrixInverse(&matWorldInverse, NULL, &stModel->m_vGeomObj[iObjNum].get()->m_matWorld);






	for (int i = 0; i < m_iFaceCount * 3; i++) {

		vp = m_vPosList[indices[i]];

		D3DXVec3TransformCoord(&vp, &vp, &matWorldInverse);

		if (m_vNorList.size() != 0)
			vn = m_vNorList[indices[i]];
		else
			vn = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		if (m_vColList.size() != 0)
			vc = D3DXVECTOR4(m_vColList[indices[i]].x, m_vColList[indices[i]].y, m_vColList[indices[i]].z, 1.0f);
		else
			vc = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

		if (m_vTextureIndex.size() != 0)
			vt = D3DXVECTOR2(m_vTexList[Texindices[i]].x, m_vTexList[Texindices[i]].y);
		else
			vt = D3DXVECTOR2(1.0f, 1.0f);

		if (stModel->m_vMaterial[0]->m_vSubMaterial.size() == 0) {
			stModel->m_vGeomObj[iObjNum].get()->m_vObj[0].get()->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
		}
		else {
			int j = i / 3;
			stModel->m_vGeomObj[iObjNum].get()->m_vObj[m_vSubMtlIndex[j]]->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
		}
	}



	delete[] Texindices;
	delete[] indices;
}
*/
bool GGbsParser::GetTrackListFromString(GGbsModel* stModel, GbsTrackType TrackType)
{
	//GAnimTrack Track;
	

	while (!feof(m_pStream))
	{
		SaveFilePosition();
		if (!_tcsicmp(m_pString, L"}")) {

			_fgetts(m_pBuffer, 256, m_pStream);
			_stscanf(m_pBuffer, _T("%s"), m_pString);

			if (!_tcsicmp(m_pString, L"}")) {
				RestoreFilePosition();
				break;
			}
			else {
				RestoreFilePosition();
			}
		}


		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);







		if (!_tcsicmp(m_pString, L"}"))
			break;

		if (TrackType == POS_SAMPLE_TRACK)
		{
			vector<shared_ptr<GAnimTrack>>& vTrack = stModel->m_vGeomObj[m_iObjCount].get()->m_vPosTrack;
			auto pTrack = make_shared<GAnimTrack>();
			ST_ANI_POS stPosData;
			//GetDataFromFileLoop(L"*CONTROL_POS_SAMPLE", &stPosData, ANI_POS_DATA);
			GetData(&stPosData, ANI_POS_DATA);
			pTrack.get()->vecVector = stPosData.vecPos;
			pTrack.get()->iTick = stPosData.iTick;
			vTrack.push_back(pTrack);
			//_stscanf(GetNextTokenString(), _T("%s%d%f%f%f"), m_pString, &Track.iTick,
			//	&Track.vVector.x,
			//	&Track.vVector.z,
			//	&Track.vVector.y);
			if (vTrack.size() > 1) {
				vTrack[vTrack.size() - 1].get()->pPrev = vTrack[vTrack.size() - 2].get();
				vTrack[vTrack.size() - 2].get()->pNext = vTrack[vTrack.size() - 1].get();
				vTrack[vTrack.size() - 1].get()->pNext = NULL;
			}
			else if (vTrack.size() <= 1) {
				vTrack[vTrack.size() - 1].get()->pPrev = NULL;
				vTrack[vTrack.size() - 1].get()->pNext = NULL;
			}
		}
		else if (TrackType == ROT_SAMPLE_TRACK)
		{
			vector<shared_ptr<GAnimTrack>>& vTrack = stModel->m_vGeomObj[m_iObjCount].get()->m_vRotTrack;
			auto pTrack = make_shared<GAnimTrack>();
			ST_ANI_ROT stRotData;
			GetData(&stRotData, ANI_ROT_DATA);
			//pTrack.get()->qRotate.x = stRotData.vecRot.x;
			//pTrack.get()->qRotate.y = stRotData.vecRot.y;
			//pTrack.get()->qRotate.z = stRotData.vecRot.z;
			//pTrack.get()->qRotate.w = stRotData.vecRot.w;

			pTrack.get()->iTick = stRotData.iTick;

			// 임의의 축과 각을 쿼터니언으로 변환
			D3DXQuaternionRotationAxis(&pTrack.get()->qRotate, &D3DXVECTOR3(stRotData.vecRot.x, stRotData.vecRot.y, stRotData.vecRot.z), stRotData.vecRot.w);



			vTrack.push_back(pTrack);

			if (vTrack.size() > 1) {
				vTrack[vTrack.size() - 1].get()->pPrev = vTrack[vTrack.size() - 2].get();
				vTrack[vTrack.size() - 2].get()->pNext = vTrack[vTrack.size() - 1].get();
				vTrack[vTrack.size() - 1].get()->pNext = NULL;
			}
			else if(vTrack.size() ==1 ){
				vTrack[vTrack.size() - 1].get()->pPrev = NULL;
				vTrack[vTrack.size() - 1].get()->pNext = NULL;

				//D3DXQuaternionMultiply(&pTrack->qRotate, &stModel->m_vGeomObj[m_iObjCount].get()->m_qRotation, &pTrack->qRotate);
			}

			// 이전트랙의 쿼터니온과 누적시킴.
			
			if (pTrack->pPrev != NULL)
			{
				D3DXQuaternionMultiply(&pTrack->qRotate, &pTrack->pPrev->qRotate, &pTrack->qRotate);
			}


			//_stscanf(GetNextTokenString(), _T("%s%d%f%f%f%f"), m_pString, &Track.iTick,
			//	&Track.qRotate.x, &Track.qRotate.z, &Track.qRotate.y, &Track.qRotate.w);
		}
		else if (TrackType == SCL_SAMPLE_TRACK)
		{
			vector<shared_ptr<GAnimTrack>>& vTrack = stModel->m_vGeomObj[m_iObjCount].get()->m_vSclTrack;
			auto pTrack = make_shared<GAnimTrack>();
			ST_ANI_SCL stSclData;
			GetData(&stSclData, ANI_SCL_DATA);
			pTrack.get()->vecVector = stSclData.vecVec;
			//pTrack.get()->qRotate.x = stSclData.vecRot.x;
			//pTrack.get()->qRotate.y = stSclData.vecRot.y;
			//pTrack.get()->qRotate.z = stSclData.vecRot.z;
			//pTrack.get()->qRotate.w = stSclData.vecRot.w;
			pTrack.get()->iTick = stSclData.iTick;

			// 임의의 축과 각을 쿼터니언으로 변환
			D3DXQuaternionRotationAxis(&pTrack.get()->qRotate, &D3DXVECTOR3(stSclData.vecRot.x, stSclData.vecRot.y, stSclData.vecRot.z), stSclData.vecRot.w);


			vTrack.push_back(pTrack);

			if (vTrack.size() > 1) {
				vTrack[vTrack.size() - 1].get()->pPrev = vTrack[vTrack.size() - 2].get();
				vTrack[vTrack.size() - 2].get()->pNext = vTrack[vTrack.size() - 1].get();
				vTrack[vTrack.size() - 1].get()->pNext = NULL;
			}
			else if (vTrack.size() == 1) {
				vTrack[vTrack.size() - 1].get()->pPrev = NULL;
				vTrack[vTrack.size() - 1].get()->pNext = NULL;



//				D3DXQuaternionMultiply(&pTrack->qRotate, &stModel->m_vGeomObj[m_iObjCount].get()->m_vObj[0].get()->m_qRotation, &pTrack->qRotate);


			}

			// 이전트랙의 쿼터니온과 누적시킴.

			//if (pTrack->pPrev != NULL)
			//{
			//	D3DXQuaternionMultiply(&pTrack->qRotate, &pTrack->pPrev->qRotate, &pTrack->qRotate);
			//}




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

int		GGbsParser::GetAnimationDataFromFile(GGbsModel* stModel) {


	int iSize = sizeof(g_pGbsAniDataTokens) / sizeof(g_pGbsAniDataTokens[0]);

	while (!feof(m_pStream))
	{

		SaveFilePosition();
		if (!_tcsicmp(m_pString, L"}")) {

			_fgetts(m_pBuffer, 256, m_pStream);
			_stscanf(m_pBuffer, _T("%s"), m_pString);

			if (!_tcsicmp(m_pString, L"}") /*||!_tcsicmp(m_pString, L"}")*/) {
				RestoreFilePosition();
				break;
			}
			else {
				RestoreFilePosition();
			}
		}

		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf(m_pBuffer, _T("%s"), m_pString);





		for (int i = 0; i < iSize; i++) {


			if (!_tcsicmp(m_pString, g_pGbsAniDataTokens[i]))
			{
				switch (i)
				{
				case ANI_CONTROL_POS_TCB:
				case ANI_CONTROL_POS_BEZIER:
				case ANI_CONTROL_POS_TRACK:
				{
					GetTrackListFromString(stModel, POS_SAMPLE_TRACK);
				}
				break;
				case ANI_CONTROL_ROT_TCB:
				case ANI_CONTROL_ROT_BEZIER:
				case ANI_CONTROL_ROT_TRACK:
				{
					GetTrackListFromString(stModel, ROT_SAMPLE_TRACK);
				}
				break;
				case ANI_CONTROL_SCALE_TCB:
				case ANI_CONTROL_SCALE_BEZIER:
				case ANI_CONTROL_SCALE_TRACK:
				{
					GetTrackListFromString(stModel, SCL_SAMPLE_TRACK);
				}
				break;

				}
			}
		}
		//fgetc( m_pStream);
	}


	return 0;
}
//int		GAseParser::GetHelperObjDataFromFile(GAseModel* stModel) {
//	bool	bLoop = true;
//
//	int iSize = sizeof(g_pAseGeomObjectTokens) / sizeof(g_pAseGeomObjectTokens[0]);
//
//	while (!feof(m_pStream) && bLoop)
//	{
//		_fgetts(m_pBuffer, 256, m_pStream);
//		_stscanf(m_pBuffer, _T("%s"), m_pString);
//
//		for (int i = 0; i < iSize; i++) {
//
//			if (!_tcsicmp(m_pString, g_pAseGeomObjectTokens[i]))
//			{
//
//			}
//
//		}
//	}
//	return 0;
//}
int		GGbsParser::GetGeomObjDataFromFile(GGbsModel* stModel) {

	bool	bLoop = true;

	
	if (_tcsicmp(m_pString, L"*HELPEROBJECT")) {
		SaveFilePosition();

		while (!feof(m_pStream))
		{
			_fgetts(m_pBuffer, 256, m_pStream);
			_stscanf(m_pBuffer, _T("%s"), m_pString);

			if (!_tcsicmp(m_pString, L"*MATERIAL_REF")){
				GetData(&stModel->m_vGeomObj[m_iObjCount].get()->m_iMaterial_Ref, INT_DATA);
				break;
			}
			
		}
		RestoreFilePosition();
	}



	if (stModel->m_vMaterial[0]->m_vSubMaterial.size() == 0) {
		stModel->m_vGeomObj[m_iObjCount].get()->m_vObj.push_back(make_shared<GGbsObj>());
	}
	else if(_tcsicmp(m_pString, L"*HELPEROBJECT")) {
		for (int i = 0; i< stModel->m_vMaterial[    stModel->m_vGeomObj[m_iObjCount].get()->m_iMaterial_Ref     ]->m_vSubMaterial.size(); i++)
			stModel->m_vGeomObj[m_iObjCount].get()->m_vObj.push_back(make_shared<GGbsObj>());
	}


	int iSize = sizeof(g_pGbsGeomObjectTokens) / sizeof(g_pGbsGeomObjectTokens[0]);

	while (!feof( m_pStream) && bLoop)
	{
		_fgetts( m_pBuffer, 256,  m_pStream);
		_stscanf( m_pBuffer, _T("%s"),  m_pString);


		for (int i = 0; i < iSize; i++) {

			//if (i == ANIMATION) {
			//	SaveFilePosition()
			//}

			if (!_tcsicmp( m_pString, g_pGbsGeomObjectTokens[i]))
			{
				switch (i)
				{
				case NODE_PARENT:
				{
					GetData(&(stModel->m_vGeomObj[m_iObjCount].get()->m_szParentName), STRING_DATA);
				}
				break;
				case NODE_NAME:
				{

					GetData(&(stModel->m_vGeomObj[m_iObjCount].get()->m_szName), STRING_DATA);

				}
				break;
				case NODE_TM:
				{
					D3DXVECTOR3 vecROW0;
					D3DXVECTOR3 vecROW1;
					D3DXVECTOR3 vecROW2;
					D3DXVECTOR3 vecROW3;

					GetDataFromFileLoop(g_pGbsNodeTmTokens[0], &vecROW0, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[1], &vecROW1, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[2], &vecROW2, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[3], &vecROW3, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[4], &stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_POS, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[5], &stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_ROTAXIS, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[6], &stModel->m_vGeomObj[m_iObjCount].get()->m_fTM_ROTANGLE, FLOAT_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[7], &stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_SCALE, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[8], &stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_SCALE_AXIS, VERTEX_DATA);
					GetDataFromFileLoop(g_pGbsNodeTmTokens[9], &stModel->m_vGeomObj[m_iObjCount].get()->m_fTM_SCALEAXISANG, FLOAT_DATA);

					// 임의의 축과 각을 쿼터니언으로 변환
					//D3DXQUATERNION qRotation;
					D3DXQuaternionRotationAxis(&stModel->m_vGeomObj[m_iObjCount].get()->m_qRotation, &stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_ROTAXIS, stModel->m_vGeomObj[m_iObjCount].get()->m_fTM_ROTANGLE);
					D3DXMatrixRotationQuaternion(&stModel->m_vGeomObj[m_iObjCount].get()->m_matWldRotate, &stModel->m_vGeomObj[m_iObjCount].get()->m_qRotation);

					//스케일축을 중심으로 회전하는 값이 스테일 값이므로 
					//스케일축의 회전만큼을 먼저 반대로 회전한 이후
					//스케일값을 적용시키고 그 다음에 
					//다시 스케일축만큼을 회전시켜 원상복귀 시킨다.
					D3DXMatrixScaling(&stModel->m_vGeomObj[m_iObjCount].get()->m_matWldScale, stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_SCALE.x, stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_SCALE.y, stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_SCALE.z);


					// 스케일축의 행렬과 그 역행렬을 구한다.
					D3DXMATRIX matRotation, matRotationInv;
					D3DXMatrixRotationAxis(&matRotation, &stModel->m_vGeomObj[m_iObjCount].get()->m_vecTM_SCALE_AXIS, stModel->m_vGeomObj[m_iObjCount].get()->m_fTM_SCALEAXISANG);
					D3DXMatrixInverse(&matRotationInv, NULL, &matRotation);

					//스케일 최종.
					stModel->m_vGeomObj[m_iObjCount].get()->m_matWldScale = matRotationInv * stModel->m_vGeomObj[m_iObjCount].get()->m_matWldScale  * matRotation;




					//GetData(&(stModel->m_vGeomObj[m_iObjCount].get()->m_vObj[0].get()->m_szName), STRING_DATA);

					D3DXMatrixIdentity(&(stModel->m_vGeomObj[m_iObjCount].get()->m_matWld));

					stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._11 = vecROW0.x; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._12 = vecROW0.y; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._13 = vecROW0.z;
					stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._31 = vecROW1.x; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._32 = vecROW1.y; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._33 = vecROW1.z;
					stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._21 = vecROW2.x; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._22 = vecROW2.y; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._23 = vecROW2.z;
					stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._41 = vecROW3.x; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._42 = vecROW3.y; stModel->m_vGeomObj[m_iObjCount].get()->m_matWld._43 = vecROW3.z;
				}
				break;
				case MESH:
				{
					GetMeshDataFromFile(stModel);
				}
				break;

				case TM_ANIMATION:
				{
					stModel->m_vGeomObj[m_iObjCount].get()->m_bHasAniTrack = true;
					stModel->m_fFrameSpeed = stModel->m_stScene.m_iFrameSpeed;
					stModel->m_fTickPerFrame = stModel->m_stScene.m_iTicksPerFrame;
					stModel->m_fLastFrame = stModel->m_stScene.m_iLastFrame;
					GetAnimationDataFromFile(stModel);
					bLoop = false;
					m_iObjCount++;
				}
				break;
				case MATERIAL_REF:
				{
					//GetData(&stModel->m_vGeomObj[m_iObjCount].get()->m_iMaterial_Ref, INT_DATA);
					bLoop = false;
					m_iObjCount++;
				}
				break;
				case BOUNDINGBOX_MIN:
				{
					GetData(&stModel->m_vGeomObj[m_iObjCount].get()->m_vecBoundingboxMin, VERTEX_DATA);
				}
				break;
				case BOUNDINGBOX_MAX:
				{
					GetData(&stModel->m_vGeomObj[m_iObjCount].get()->m_vecBoundingboxMax, VERTEX_DATA);

					SaveFilePosition();
					_fgetts(m_pBuffer, 256, m_pStream);
					_stscanf(m_pBuffer, _T("%s"), m_pString);

					if (!_tcsicmp(m_pString, L"*TM_ANIMATION")){
						RestoreFilePosition();
						break;
					}
					else {
						bLoop = false;
						m_iObjCount++;
					}
				}
				break;

				}
			}
		}
		//fgetc( m_pStream);
	}


	return 0;

}
int		GGbsParser::GetDataFromFile(GGbsModel* stModel ){

	int iMtlCnt = 0,iObjCnt = 0;

	int iSize = sizeof(g_pGbsItems) / sizeof(g_pGbsItems[0]);

	while (!feof( m_pStream))
	{
		_fgetts( m_pBuffer, 256,  m_pStream);
		_stscanf( m_pBuffer, _T("%s"),  m_pString);


		for (int i = 0; i < iSize; i++) {

			if (!_tcsicmp( m_pString, g_pGbsItems[i]))
			{
				switch (i)
				{
				case SCENE:
				{
					/*
					//iVersion, iFirstFrame, iLastFrame,  iFrameSpeed, iTickPerFrame, iNumMtl, iNumObject
					#SCENE 100 0 100 30 160 1 1
					*/
					int iVersion = 0, iFirstFrame = 0, iLastFrame = 0, iFrameSpeed = 0, iTickPerFrame = 0, iNumMtl = 0, iNumObject = 0;

					_stscanf(m_pBuffer, _T("%s %d %d %d %d %d %d %d"),
						m_pString,
						&iVersion, &iFirstFrame, &iLastFrame, &iFrameSpeed, &iTickPerFrame, &iNumMtl, &iNumObject
						);

					stModel->m_stScene.m_iFrame = iFirstFrame;
					stModel->m_stScene.m_iLastFrame = iLastFrame;
					stModel->m_stScene.m_iFrameSpeed = iFrameSpeed;
					stModel->m_stScene.m_iTicksPerFrame = iTickPerFrame;

					for (int j = 0; j < iNumObject; j++) {
						auto gbsgeom = make_shared<GGbsGeom>();
						gbsgeom->m_iType = 0;
						stModel->m_vGeomObj.push_back(gbsgeom);
					}

					for (int j = 0; j < iNumMtl; j++) {
						auto material = make_shared<GGbsMaterial>();
						stModel->m_vMaterial.push_back(material);
					}
				}
				break;
				case MATERIAL:
				{
					int iDiffuse = 0;
						/*
						//itMtl, m_gMtlList[itMtl].subMtls.size(), m_gMtlList[itMtl].texlist.size()
						#MATERIAL 0 0 1
						*/
					int iMtl = 0, iSubMtlSize = 0, iTexListSize = 0;
					_stscanf(m_pBuffer, _T("%s %d %d %d"),
						m_pString,
						&iMtl, &iSubMtlSize, &iTexListSize
						);


					if (iTexListSize == 1) {

						_fgetts(m_pBuffer, 256, m_pStream);
						_stscanf(m_pBuffer, _T("%d %s"),
							&iDiffuse,
							&stModel->m_vMaterial[iMtlCnt].get()->m_szMapDiffuse
							);


						TCHAR strDir[MAX_PATH] = L"data\\";
						_tcsncat(strDir, stModel->m_vMaterial[iMtlCnt].get()->m_szMapDiffuse, _tcsclen(stModel->m_vMaterial[iMtlCnt].get()->m_szMapDiffuse));

						_tcscpy(stModel->m_vMaterial[iMtlCnt].get()->m_szMapDiffuse, strDir);

					}
					else {
						/*
						#SUBMATERIAL 0 0 1
						1 0_st02_sc00_g04.tga
						#SUBMATERIAL 1 0 1
						1 0_st02_sc00_g00.tga
						#SUBMATERIAL 2 0 1
						1 0_st02_sc00_g01.tga
						#SUBMATERIAL 3 0 1
						1 0_st02_sc00_g03.tga
						#SUBMATERIAL 4 0 1
						1 0_st02_sc00_g02.tga
						*/
						for (int j = 0; j < iSubMtlSize; j++) {
							auto submaterial = make_shared<GGbsMaterial>();
							stModel->m_vMaterial[iMtlCnt].get()->m_vSubMaterial.push_back(submaterial);
						}
						for (int j = 0; j < iSubMtlSize; j++) {

							int i_temp_Mtl = 0, i_temp_SubMtlSize = 0, i_temp_TexListSize = 0;

							_fgetts(m_pBuffer, 256, m_pStream);
							_stscanf(m_pBuffer, _T("%s %d %d %d"),
								m_pString,
								&i_temp_Mtl, &i_temp_SubMtlSize, &i_temp_TexListSize
								);

							_fgetts(m_pBuffer, 256, m_pStream);
							_stscanf(m_pBuffer, _T("%d %s"),
								&iDiffuse,
								&stModel->m_vMaterial[iMtlCnt].get()->m_vSubMaterial[j].get()->m_szMapDiffuse
								);

							TCHAR strDir[MAX_PATH] = L"data\\";
							_tcsncat(strDir, stModel->m_vMaterial[iMtlCnt].get()->m_vSubMaterial[j].get()->m_szMapDiffuse, _tcsclen(stModel->m_vMaterial[iMtlCnt].get()->m_vSubMaterial[j].get()->m_szMapDiffuse));

							_tcscpy(stModel->m_vMaterial[iMtlCnt].get()->m_vSubMaterial[j].get()->m_szMapDiffuse, strDir);
						}
					}	
					iMtlCnt++;
				}
				break;

				case OBJECT:
				{
					int iObj = 0;
					/*
					//iObj, m_gObjectList[iObj].nodeName, m_gObjectList[iObj].nodeParentName
					#OBJECT 0 Box01 Scene_Root
					*/
					_stscanf(m_pBuffer, _T("%s %d %s %s"),
						m_pString,
						&iObj,
						&stModel->m_vGeomObj[iObjCnt].get()->m_szName,
						&stModel->m_vGeomObj[iObjCnt].get()->m_szParentName
						);

					_fgetts(m_pBuffer, 256, m_pStream);
					_stscanf(m_pBuffer, _T("%d"),
						&stModel->m_vGeomObj[iObjCnt].get()->m_iMaterial_Ref);

					//GetGeomObjDataFromFile(stModel);
				}
				break;

				case WORLD:
				{
					/*
					//m_gObjectList[iObj].matWorld._11  ~ 44
					#WORLD     1.0000     0.0000     0.0000     0.0000      0.0000     1.0000     0.0000     0.0000      0.0000     0.0000     1.0000     0.0000     -0.0773     0.0000    -0.1949     1.0000 
					*/
					_stscanf(m_pBuffer, _T("%s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f"),
						m_pString,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._11,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._12,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._13,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._14,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._21,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._22,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._23,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._24,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._31,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._32,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._33,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._34,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._41,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._42,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._43,
						&stModel->m_vGeomObj[iObjCnt].get()->m_matWld._44
						);
				}
				break;

				case TRIANGLE:
				{
					int iSubMtlSize = 0;


					_stscanf(m_pBuffer, _T("%s %d"),
						m_pString,
						&iSubMtlSize
						);

					if (iSubMtlSize == 0) iSubMtlSize = 1;

					for (int j = 0; j < iSubMtlSize; j++) {
						auto gbsobj = make_shared<GGbsObj>();
						stModel->m_vGeomObj[iObjCnt].get()->m_vObj.push_back(gbsobj);
					}

					for (int j = 0; j < iSubMtlSize; j++) {

						int iMtlNum = 0, iVertexSize = 0, iIndexSize = 0;

						_fgetts(m_pBuffer, 256, m_pStream);
						_stscanf(m_pBuffer, _T("%d %d %d"),
							&iMtlNum,
							&iVertexSize,
							&iIndexSize
							);

						/*stModel->m_vGeomObj[iObjCnt].get()->m_iMaterial_Ref = iMtlNum;*/

						for (int k = 0; k < iVertexSize; k++) {

							D3DXVECTOR3		vp;
							D3DXVECTOR3		vn;
							D3DXVECTOR4		vc;
							D3DXVECTOR2     vt;

							_fgetts(m_pBuffer, 256, m_pStream);
							_stscanf(m_pBuffer, _T("%f %f %f %f %f %f %f %f %f %f %f %f"),
								&vp.x, &vp.y, &vp.z,
								&vn.x, &vn.y, &vn.z,
								&vc.x, &vc.y, &vc.z, &vc.w,
								&vt.x, &vt.y
								);

							if (iMtlNum == -1) {
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[0].get()->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
							}
							else {
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[iMtlNum]->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
							}
						}


						for (int k = 0; k < iIndexSize / 3; k++) {
							int iIndex1 = 0, iIndex2 = 0, iIndex3 = 0;
							_fgetts(m_pBuffer, 256, m_pStream);
							_stscanf(m_pBuffer, _T("%d %d %d"), &iIndex1, &iIndex2, &iIndex3);
							if (iMtlNum == -1) {
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[0].get()->m_vIndexList.push_back(iIndex1);
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[0].get()->m_vIndexList.push_back(iIndex2);
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[0].get()->m_vIndexList.push_back(iIndex3);
							}
							else {
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[iMtlNum]->m_vIndexList.push_back(iIndex1);
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[iMtlNum]->m_vIndexList.push_back(iIndex2);
								stModel->m_vGeomObj[iObjCnt].get()->m_vObj[iMtlNum]->m_vIndexList.push_back(iIndex3);
							}
						}
					}

					iObjCnt++;
					//GetGeomObjDataFromFile(stModel);
				}

				break;
				}
			}
		}
		//fgetc( m_pStream);
	}



	return 0;
}


void    GGbsParser::SetPnctData(GGbsModel* stModel, int iObjNum) {

	D3DXVECTOR3		vp;
	D3DXVECTOR3		vn;
	D3DXVECTOR4		vc;
	D3DXVECTOR2     vt;
	D3DXMATRIX		matWorldInverse;


	// Create index buffer
	WORD* indices;
	indices = (WORD *)malloc(sizeof(WORD) * m_iFaceCount * 3);

	if(m_vIndex.size() != 0){
		for (int i = 0; i < m_iFaceCount * 3; i++) {

			if (i == 0 || i % 3 == 0) {
				indices[i] = { (WORD)(m_vIndex[i]) };
			}
			else if (i == 1 || i % 3 == 1) {
				indices[i] = { (WORD)(m_vIndex[i + 1]) };
			}
			else if (i == 2 || i % 3 == 2) {
				indices[i] = { (WORD)(m_vIndex[i - 1]) };
			}
		}
	}


	WORD* Texindices;
	Texindices = (WORD *)malloc(sizeof(WORD) * m_iFaceCount * 3);


	if(m_vTextureIndex.size() !=0 ){
		for (int i = 0; i < m_iFaceCount * 3; i++) {

			if (i == 0 || i % 3 == 0) {
				Texindices[i] = { (WORD)(m_vTextureIndex[i]) };
			}
			else if (i == 1 || i % 3 == 1) {
				Texindices[i] = { (WORD)(m_vTextureIndex[i + 1]) };
			}
			else if (i == 2 || i % 3 == 2) {
				Texindices[i] = { (WORD)(m_vTextureIndex[i - 1]) };
			}
		}
	}

	D3DXMatrixInverse(&matWorldInverse, NULL, &stModel->m_vGeomObj[iObjNum].get()->m_matWld);






		for (int i = 0; i < m_iFaceCount * 3; i++) {

			vp = m_vPosList[indices[i]];

			D3DXVec3TransformCoord(&vp, &vp, &matWorldInverse);

			if (m_vNorList.size() != 0)
				vn = m_vNorList[indices[i]];
			else
				vn = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

			if (m_vColList.size() != 0)
				vc = D3DXVECTOR4(m_vColList[indices[i]].x, m_vColList[indices[i]].y, m_vColList[indices[i]].z, 1.0f);
			else
				vc = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

			if (m_vTextureIndex.size() != 0)
				vt = D3DXVECTOR2(m_vTexList[Texindices[i]].x, m_vTexList[Texindices[i]].y);
			else
				vt = D3DXVECTOR2(1.0f, 1.0f);

			if (stModel->m_vMaterial[0]->m_vSubMaterial.size() == 0) {
				stModel->m_vGeomObj[iObjNum].get()->m_vObj[0].get()->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
			}
			else {
				int j = i / 3;

				if (m_vSubMtlIndex[j] == 255)
					continue;

				stModel->m_vGeomObj[iObjNum].get()->m_vObj[    m_vSubMtlIndex[j]    ]->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
			}
		}



	delete[] Texindices;
	delete[] indices;

}

GGbsParser::GGbsParser()
{
	ResetVariables();
}


GGbsParser::~GGbsParser()
{
	ResetVariables();
}
