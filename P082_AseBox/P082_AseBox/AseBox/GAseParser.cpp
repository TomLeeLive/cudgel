#include "_stdafx.h"

int		GAseParser::GetMeshDataFromFile(GAseModel* stModel, GAseObj* aseobj) {


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
					 GetData(&(aseobj->m_iPosCount), INT_DATA);
					break;
				}
				break;
				case MESH_NUMFACES:
				{
					 GetData(&(aseobj->m_iFaceCount), INT_DATA);
				}
				break;
				case MESH_VERTEX_LIST:
				{
					D3DXVECTOR3 pPos;
					for (int i = 0; i < aseobj->m_iPosCount; i++) {
						//pPos = new D3DXVECTOR3;
						 GetDataFromFileLoop(L"*MESH_VERTEX", &pPos, MESH_VERTEX_DATA);
						aseobj->m_vPosList.push_back(pPos);
					}
				}
				break;
				case MESH_FACE_LIST:
				{
					ST_INT_THREE stMeshFace;
					for (int i = 0; i < aseobj->m_iFaceCount; i++) {
						 GetDataFromFileLoop(L"*MESH_FACE", &stMeshFace, MESH_FACE_DATA);
						aseobj->m_vIndex.push_back(stMeshFace.index1);
						aseobj->m_vIndex.push_back(stMeshFace.index2);
						aseobj->m_vIndex.push_back(stMeshFace.index3);
					}
				}
				break;
				case MESH_NUMTVERTEX:
				{
					 GetData(&(aseobj->m_iTexVerCount), INT_DATA);
				}
				break;
				case MESH_TVERTLIST:
				{
					D3DXVECTOR3 vUVData; float fTemp;
					for (int i = 0; i < aseobj->m_iTexVerCount; i++) {
						 GetDataFromFileLoop(L"*MESH_TVERT", &vUVData, MESH_VERTEX_DATA);

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
					 GetData(&(aseobj->m_iTexFaceCount), INT_DATA);
				}
				break;
				case MESH_TFACELIST:
				{
					ST_INT_THREE stFaceList;
					for (int i = 0; i < aseobj->m_iTexFaceCount; i++) {
						 GetDataFromFileLoop(L"*MESH_TFACE", &stFaceList, FACE_DATA);
						aseobj->m_vTextureIndex.push_back(stFaceList.index1);
						aseobj->m_vTextureIndex.push_back(stFaceList.index2);
						aseobj->m_vTextureIndex.push_back(stFaceList.index3);
					}
				}
				break;
				case MESH_NUMCVERTEX:
				{
					 GetData(&(aseobj->m_iColorVerCount), INT_DATA);
				}
				break;
				case MESH_CVERTLIST:
				{
					D3DXVECTOR3 vColor;
					for (int i = 0; i < aseobj->m_iColorVerCount; i++) {
						 GetDataFromFileLoop(L"*MESH_VERTCOL", &vColor, MESH_VERTEX_DATA);
						aseobj->m_vColList.push_back(vColor);
					}
				}
				break;
				case MESH_NORMALS:
				{
					D3DXVECTOR3 vNormal;
					for (int i = 0; i < aseobj->m_iTexFaceCount; i++) {
						 GetDataFromFileLoop(L"*MESH_FACENORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vFaceNormal.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vNorList.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vNorList.push_back(vNormal);
						 GetDataFromFileLoop(L"*MESH_VERTEXNORMAL", &vNormal, MESH_VERTEX_DATA);
						aseobj->m_vNorList.push_back(vNormal);
					}
				}
				break;

				}
			}
		}
		//fgetc( m_pStream);
	}

	return -1;
}
int		GAseParser::GetObjDataFromFile(GAseModel* stModel) {

	//GAseObj* aseobj = new GAseObj;
	auto aseobj = make_shared<GAseObj>();

	int iSize = sizeof(g_pAseObjTokens) / sizeof(g_pAseObjTokens[0]);

	while (!feof( m_pStream))
	{
		_fgetts( m_pBuffer, 256,  m_pStream);
		_stscanf( m_pBuffer, _T("%s"),  m_pString);


		for (int i = 0; i < iSize; i++) {


			if (!_tcsicmp( m_pString, g_pAseObjTokens[i]))
			{
				switch (i)
				{

				case NODE_NAME:
				{

					 GetData(&(aseobj->m_szName), STRING_DATA);
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

					D3DXMatrixIdentity(&aseobj->m_matWorld);

					aseobj->m_matWorld._11 = vecROW0.x; aseobj->m_matWorld._12 = vecROW0.y; aseobj->m_matWorld._13 = vecROW0.z;
					aseobj->m_matWorld._31 = vecROW1.x; aseobj->m_matWorld._32 = vecROW1.y; aseobj->m_matWorld._33 = vecROW1.z;
					aseobj->m_matWorld._21 = vecROW2.x; aseobj->m_matWorld._22 = vecROW2.y; aseobj->m_matWorld._23 = vecROW2.z;
					aseobj->m_matWorld._41 = vecROW3.x; aseobj->m_matWorld._42 = vecROW3.y; aseobj->m_matWorld._43 = vecROW3.z;

				}
				break;
				case MESH:
				{

					GetMeshDataFromFile(stModel, aseobj.get());


				}
				break;

				}
			}
		}
		//fgetc( m_pStream);
	}

	stModel->m_vObj.push_back(aseobj);
	return -1;

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
					//GAseMaterial* material = new GAseMaterial;
					auto material = make_shared<GAseMaterial>();
					 GetDataFromFileLoop(g_pAseMaterialTokens[0], &(material->m_szName), STRING_DATA);
					 GetDataFromFileLoop(g_pAseMaterialTokens[1], &(material->m_vecAmbient), VERTEX_DATA);
					 GetDataFromFileLoop(g_pAseMaterialTokens[2], &(material->m_vecDiffuse), VERTEX_DATA);
					 GetDataFromFileLoop(g_pAseMaterialTokens[3], &(material->m_vecSpecular), VERTEX_DATA);
					 GetDataFromFileLoop(g_pAseMaterialTokens[4], &(material->m_szMapDiffuse), STRING_DATA);
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

	return -1;
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

	// Create vertex buffer
	PNCT_VERTEX* vertices;
	vertices = (PNCT_VERTEX *)malloc(sizeof(PNCT_VERTEX) * stModel->m_vObj[0]->m_iFaceCount * 3);


	for (int i = 0; i < stModel->m_vObj[0]->m_iFaceCount * 3; i++) {

		vp = stModel->m_vObj[0]->m_vPosList[indices[i]];

		D3DXVec3TransformCoord(&vp, &vp, &matWorldInverse);

		vn = stModel->m_vObj[0]->m_vNorList[indices[i]];

		if (stModel->m_vObj[0]->m_iColorVerCount != 0)
			vc = D3DXVECTOR4(stModel->m_vObj[0]->m_vColList[indices[i]].x, stModel->m_vObj[0]->m_vColList[indices[i]].y, stModel->m_vObj[0]->m_vColList[indices[i]].z, 1.0f);
		else
			vc = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

		vt = D3DXVECTOR2(stModel->m_vObj[0]->m_vTexList[Texindices[i]].x, stModel->m_vObj[0]->m_vTexList[Texindices[i]].y);
		stModel->m_vObj[0]->m_vPnctVertex.push_back(PNCT_VERTEX(vp, vn, vc, vt));
	}

	delete[] Texindices;
	delete[] indices;
	delete[] vertices;
}

GAseParser::GAseParser()
{
}


GAseParser::~GAseParser()
{
}
