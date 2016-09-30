#include "GbsWriter.h"


#define SINGLE_QUOTE  39 // (')
#define CTL_CHARS     31 //시스템 제어 문자



struct AScendingSort
{
	bool operator()(GTri& a, GTri& b)
	{
		return a.iSubIndex < b.iSubIndex;
	}
};

int g_iSubIndex = -1;
struct IsSubEqual
{
	bool operator()(GTri& a)
	{
		return a.iSubIndex == g_iSubIndex;
	}
};

int GbsWriter::IsEqulVertexList(vector<PNCT_VERTEX>& UniqueVertex,
	PNCT_VERTEX& vertex)
{
	for (int iCnt = 0; iCnt < UniqueVertex.size(); iCnt++)
	{
		if (EqualPoint3(UniqueVertex[iCnt].p, vertex.p) &&
			EqualPoint3(UniqueVertex[iCnt].n, vertex.n) &&
			EqualPoint4(UniqueVertex[iCnt].c, vertex.c) &&
			EqualPoint2(UniqueVertex[iCnt].t, vertex.t))
		{
			return iCnt;
		}
	}
	return -1;
};
int GbsWriter::ExpMesh(GMesh& pMesh, int iMtl, int iAddCount)
{
	vector<PNCT_VERTEX>  UniqueVertex;
	vector<DWORD>  UniqueIndex;
	int iNumTriangle = pMesh.triList.size();
	if (iMtl >= 0)
	{
		g_iSubIndex = iMtl;
		iNumTriangle =
			std::count_if(
				pMesh.triList.begin(),
				pMesh.triList.end(),
				IsSubEqual());
	}

	for (int iFace = 0; iFace < iNumTriangle; iFace++)
	{

		for (int iVertex = 0; iVertex < 3; iVertex++)
		{
			int iPosReturn = IsEqulVertexList(UniqueVertex,
				pMesh.triList[iAddCount + iFace].v[iVertex]);
			if (iPosReturn < 0)
			{
				UniqueVertex.push_back(pMesh.triList[iAddCount + iFace].v[iVertex]);
				iPosReturn = UniqueVertex.size() - 1;
			}
			UniqueIndex.push_back(iPosReturn);
		}

	}

	_ftprintf(m_fp,
		_T("%d %d %d\n"), iMtl, UniqueVertex.size(),
		UniqueIndex.size());
	for (int iCnt = 0; iCnt < UniqueVertex.size(); iCnt++)
	{
		_ftprintf(m_fp,
			_T("%10.4f %10.4f %10.4f %10.4f  %10.4f %10.4f %10.4f %10.4f  %10.4f %10.4f %10.4f %10.4f \n"),
			UniqueVertex[iCnt].p.x,
			UniqueVertex[iCnt].p.y,
			UniqueVertex[iCnt].p.z,

			UniqueVertex[iCnt].n.x,
			UniqueVertex[iCnt].n.y,
			UniqueVertex[iCnt].n.z,

			UniqueVertex[iCnt].c.x,
			UniqueVertex[iCnt].c.y,
			UniqueVertex[iCnt].c.z,
			UniqueVertex[iCnt].c.w,

			UniqueVertex[iCnt].t.x,
			UniqueVertex[iCnt].t.y);
	}
	for (int iCnt = 0; iCnt < UniqueIndex.size(); iCnt += 3)
	{
		_ftprintf(m_fp, _T(" %d"), UniqueIndex[iCnt + 0]);
		_ftprintf(m_fp, _T(" %d"), UniqueIndex[iCnt + 1]);
		_ftprintf(m_fp, _T(" %d\n"), UniqueIndex[iCnt + 2]);
	}
	return iNumTriangle;
};



void GbsWriter::GetPosTrack(INode* pNode, GMesh& tMesh)
{
	for (int t = m_Interval.Start();
	t <= m_Interval.End();  t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t);
		Matrix3 ptm = pNode->GetParentTM(t);
		tm = tm * Inverse(ptm);

		AffineParts parts;
		decomp_affine(tm, &parts);
		GAnimTrack track;
		track.iTick = m_Interval.Start();
		track.vValue.x = parts.t.x;
		track.vValue.y = parts.t.z;
		track.vValue.z = parts.t.y;
		tMesh.posTrack.push_back(track);
	}
}
void GbsWriter::GetRotTrack(INode* pNode, GMesh& tMesh)
{
	Matrix3 tm;
	for (int t = m_Interval.Start();
	t <= m_Interval.End();  t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t);
		Matrix3 ptm = pNode->GetParentTM(t);
		tm = tm * Inverse(ptm);

		AffineParts parts;
		decomp_affine(tm, &parts);
		GAnimTrack track;
		track.iTick = m_Interval.Start();
		track.qValue.x = parts.q.x;
		track.qValue.y = parts.q.z;
		track.qValue.z = parts.q.y;
		track.qValue.w = parts.q.w;
		tMesh.rotTrack.push_back(track);
	}
}
void GbsWriter::GetSclTrack(INode* pNode, GMesh& tMesh)
{
	for (int t = m_Interval.Start();
	t <= m_Interval.End();  t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t);
		Matrix3 ptm = pNode->GetParentTM(t);
		tm = tm * Inverse(ptm);

		AffineParts parts;
		decomp_affine(tm, &parts);
		GAnimTrack track;
		track.iTick = m_Interval.Start();
		track.qValue.x = parts.u.x;
		track.qValue.y = parts.u.z;
		track.qValue.z = parts.u.y;
		track.qValue.w = parts.u.w;

		track.vValue.x = parts.k.x;
		track.vValue.y = parts.k.z;
		track.vValue.z = parts.k.y;
		tMesh.sclTrack.push_back(track);
	}

}
BOOL GbsWriter::EqualPoint3(Point3 t0, Point3 t1)
{
	if (fabs(t1.x - t0.x) > 0.001f) return FALSE;
	if (fabs(t1.y - t0.y) > 0.001f) return FALSE;
	if (fabs(t1.z - t0.z) > 0.001f) return FALSE;
	return TRUE;
}
BOOL GbsWriter::EqualPoint4(Point4 t0, Point4 t1)
{
	if (fabs(t1.x - t0.x) > 0.001f) return FALSE;
	if (fabs(t1.y - t0.y) > 0.001f) return FALSE;
	if (fabs(t1.z - t0.z) > 0.001f) return FALSE;
	if (fabs(t1.w - t0.w) > 0.001f) return FALSE;
	return TRUE;
}
BOOL GbsWriter::EqualPoint2(Point2 t0, Point2 t1)
{
	if (fabs(t1.x - t0.x) > 0.001f) return FALSE;
	if (fabs(t1.y - t0.y) > 0.001f) return FALSE;
	return TRUE;
}

BOOL GbsWriter::EqualQuat(Quat q0, Quat q1)
{
	Point3 vAxis[2];
	float  fAngle[2];
	AngAxisFromQ(q0, &fAngle[0], vAxis[0]);
	AngAxisFromQ(q1, &fAngle[1], vAxis[1]);

	if (!EqualPoint3(vAxis[0], vAxis[1]))
	{
		return FALSE;
	}
	if (fabs(fAngle[0] - fAngle[1]) > 0.001f) return FALSE;

	return TRUE;
}
BOOL GbsWriter::CheckForAnimation(INode* pNode, GMesh& tMesh,
	BOOL& IsPos,
	BOOL& IsRotate, BOOL& IsScale)
{
	Matrix3 tm = pNode->GetNodeTM(m_Interval.Start());
	Matrix3 ptm = pNode->GetParentTM(m_Interval.Start());
	tm = tm * Inverse(ptm);
	AffineParts parts;
	decomp_affine(tm, &parts);

	for (int t = m_Interval.Start() + GetTicksPerFrame();
	t <= m_Interval.End();  t += GetTicksPerFrame())
	{
		AffineParts frameAp;
		decomp_affine(tm, &frameAp);

		if (IsPos == FALSE &&!EqualPoint3(parts.t, frameAp.t))
		{
			IsPos = TRUE;
		}
		if (IsRotate == FALSE &&!EqualQuat(parts.q, frameAp.q))
		{
			IsRotate = TRUE;
		}
		if (IsScale == FALSE &&!EqualPoint3(parts.k, frameAp.k))
		{
			IsScale = TRUE;
		}
		if (IsScale == FALSE &&!EqualQuat(parts.u, frameAp.u))
		{
			IsScale = TRUE;
		}
	}
	if (IsScale == FALSE)
	{
		GAnimTrack track;
		track.iTick = m_Interval.Start();
		track.qValue.x = parts.u.x;
		track.qValue.y = parts.u.z;
		track.qValue.z = parts.u.y;
		track.qValue.w = parts.u.w;

		track.vValue.x = parts.k.x;
		track.vValue.y = parts.k.z;
		track.vValue.z = parts.k.y;
		tMesh.sclTrack.push_back(track);
	}
	if (IsRotate == FALSE)
	{
		GAnimTrack track;
		track.iTick = m_Interval.Start();
		track.qValue.x = parts.q.x;
		track.qValue.y = parts.q.z;
		track.qValue.z = parts.q.y;
		track.qValue.w = parts.q.w;
		tMesh.sclTrack.push_back(track);
	}
	if (IsPos == FALSE)
	{
		GAnimTrack track;
		track.iTick = m_Interval.Start();
		track.vValue.x = parts.t.x;
		track.vValue.y = parts.t.z;
		track.vValue.z = parts.t.y;
		tMesh.sclTrack.push_back(track);
	}
	return IsScale || IsRotate || IsPos;
}




BOOL GbsWriter::TMNegParity(Matrix3 tm)
{
	Point3 vCross = CrossProd(tm.GetRow(0), tm.GetRow(1));
	float fDot = DotProd(vCross, tm.GetRow(2));
	if (fDot < 0.0f) return TRUE;
	return FALSE;
}
Point3 GbsWriter::GetVertexNormal(Mesh* pMesh, int iFace,
	RVertex* rv)
{
	Face* f = &pMesh->faces[iFace];
	DWORD smGroup = f->smGroup;
	Point3 vertexNormal;
	// 정점을 공유하는 페이스 개수
	int numNormals = (rv->rFlags & NORCT_MASK);
	// 노말을 직접 지정했다.
	if (rv->rFlags & SPECIFIED_NORMAL)
	{
		vertexNormal = rv->rn.getNormal();
	}
	// 스무싱 작업이 되었다.
	else if (smGroup && numNormals)
	{
		if (numNormals == 1)
		{
			vertexNormal = rv->rn.getNormal();
		}
		else
		{
			for (int i = 0; i < numNormals; i++)
			{
				if (rv->ern[i].getSmGroup() & smGroup)
				{
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else // 페이스 노말을 그대로 정점에 대입했다.
	{
		vertexNormal = pMesh->getFaceNormal(iFace);
	}
	return vertexNormal;
}

void  GbsWriter::DumpPoint3(Point3& v, Point3& Vertex)
{
	Vertex.x = v.x;
	Vertex.y = v.z;
	Vertex.z = v.y;
}
void  GbsWriter::DumpPoint2(Point2& v, Point2& Vertex)
{
	Vertex.x = v.x;
	Vertex.y = 1.0f - v.y;
}
void  GbsWriter::DumpQuat(Quat& v, Quat& q)
{
	q.x = v.x;
	q.y = v.z;
	q.z = v.y;
	q.w = v.w;
}
void  GbsWriter::DumpPoint4(Point4& v, Point4& q)
{
	q.x = v.x;
	q.y = v.z;
	q.z = v.y;
	q.w = v.w;
}
TriObject* GbsWriter::GetTriObjectFromNode(
	INode* pNode,
	int iTick,
	BOOL &needDel)
{
	needDel = FALSE;
	// Object -> TriObject
	Object* pObj = pNode->EvalWorldState(iTick).obj;
	if (pObj)
	{
		if (pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
		{
			TriObject* pTri = (TriObject*)pObj->ConvertToType(
				iTick, Class_ID(TRIOBJ_CLASS_ID, 0));
			if (pObj != (Object*)pTri)
			{
				needDel = TRUE;
			}
			return pTri;
		}
	}
	return NULL;
};
void GbsWriter::GetMesh(INode* pNode, GMesh& gMesh)
{
	// INode -> TriObject -> Mesh
	BOOL needDel;
	TriObject* pTri = GetTriObjectFromNode(
		pNode,
		m_Interval.Start(),
		needDel);

	// 로칼정점 일 경우 -> 단위행렬이 아니다.
	// 월드정점 일 경우 -> 단위행렬이다.
	Matrix3 tm = pNode->GetObjTMAfterWSM(m_Interval.Start());

	int id[3];

	bool negScale = TMNegParity(tm);
	if (negScale)
	{
		id[0] = 2; id[1] = 1; id[2] = 0;
	}
	else
	{
		id[0] = 0; id[1] = 1; id[2] = 2;
	}

	if (pTri != NULL)
	{
		Mesh* pMesh = &(pTri->GetMesh());
		pMesh->buildNormals();
		gMesh.triList.resize(pMesh->numFaces);

		for (int iFace = 0; iFace < pMesh->numFaces; iFace++)
		{
			GTri tTri;

			if (pMesh->numVerts > 0)
			{
				Point3 v;
				v = tm * pMesh->verts[pMesh->faces[iFace].v[id[0]]];
				DumpPoint3(v, tTri.v[0].p);
				v = tm * pMesh->verts[pMesh->faces[iFace].v[id[2]]];
				DumpPoint3(v, tTri.v[1].p);
				v = tm * pMesh->verts[pMesh->faces[iFace].v[id[1]]];
				DumpPoint3(v, tTri.v[2].p);
			}

			if (pMesh->numTVerts > 0)
			{
				Point3 t;
				t = pMesh->tVerts[pMesh->tvFace[iFace].t[id[0]]];
				tTri.v[0].t.x = t.x;
				tTri.v[0].t.y = 1.0f - t.y;
				t = pMesh->tVerts[pMesh->tvFace[iFace].t[id[2]]];
				tTri.v[1].t.x = t.x;
				tTri.v[1].t.y = 1.0f - t.y;
				t = pMesh->tVerts[pMesh->tvFace[iFace].t[id[1]]];
				tTri.v[2].t.x = t.x;
				tTri.v[2].t.y = 1.0f - t.y;
			}
			tTri.v[0].c = Point4(1, 1, 1, 1);
			tTri.v[1].c = Point4(1, 1, 1, 1);
			tTri.v[2].c = Point4(1, 1, 1, 1);

			if (pMesh->numCVerts > 0)
			{
				Point3 c;
				c = pMesh->vertCol[pMesh->vcFace[iFace].t[id[0]]];
				tTri.v[0].c.x = c.x;
				tTri.v[0].c.y = c.y;
				tTri.v[0].c.z = c.z;
				tTri.v[0].c.w = 1.0f;
				c = pMesh->vertCol[pMesh->vcFace[iFace].t[id[2]]];
				tTri.v[1].c.x = c.x;
				tTri.v[1].c.y = c.y;
				tTri.v[1].c.z = c.z;
				tTri.v[1].c.w = 1.0f;
				c = pMesh->vertCol[pMesh->vcFace[iFace].t[id[1]]];
				tTri.v[2].c.x = c.x;
				tTri.v[2].c.y = c.y;
				tTri.v[2].c.z = c.z;
				tTri.v[2].c.w = 1.0f;
			}

			Face* f = &pMesh->faces[iFace];
			Point3 fn = pMesh->getFaceNormal(iFace);
			Point3 vn;
			RVertex* rv = pMesh->getRVertPtr(f->getVert(id[0]));
			tTri.v[0].n = GetVertexNormal(pMesh, iFace, rv);
			rv = pMesh->getRVertPtr(f->getVert(id[2]));
			tTri.v[1].n = GetVertexNormal(pMesh, iFace, rv);
			rv = pMesh->getRVertPtr(f->getVert(id[1]));
			tTri.v[2].n = GetVertexNormal(pMesh, iFace, rv);

			tTri.iSubIndex = pMesh->faces[iFace].getMatID();

			tTri.v[0].c.w = tTri.iSubIndex;
			tTri.v[1].c.w = tTri.iSubIndex;
			tTri.v[2].c.w = tTri.iSubIndex;

			gMesh.triList[iFace] = tTri;
		}
	}
	if (needDel) delete pTri;
}
void GbsWriter::GetAnimation(INode* pNode, GMesh& gMesh)
{
	BOOL IsPos = FALSE;
	BOOL IsRotate = FALSE;
	BOOL IsScale = FALSE;
	if (CheckForAnimation(pNode, gMesh, IsPos, IsRotate, IsScale))
	{
		if (IsPos == TRUE)		GetPosTrack(pNode, gMesh);
		if (IsRotate == TRUE)	GetRotTrack(pNode, gMesh);
		if (IsScale == TRUE)	GetSclTrack(pNode, gMesh);
	}
}
void  GbsWriter::DumpMatrix3(Matrix3& m,
	D3D_MATRIX& d3dWorld)
{
	Point3 row;
	row = m.GetRow(0);
	d3dWorld._11 = row.x; d3dWorld._12 = row.z; d3dWorld._13 = row.y;
	d3dWorld._14 = 0.0f;
	row = m.GetRow(2);
	d3dWorld._21 = row.x; d3dWorld._22 = row.z; d3dWorld._23 = row.y;
	d3dWorld._24 = 0.0f;
	row = m.GetRow(1);
	d3dWorld._31 = row.x; d3dWorld._32 = row.z; d3dWorld._33 = row.y;
	d3dWorld._34 = 0.0f;
	row = m.GetRow(3);
	d3dWorld._41 = row.x; d3dWorld._42 = row.z; d3dWorld._43 = row.y;
	d3dWorld._44 = 1.0f;
};

TSTR GbsWriter::FixupName(TSTR name)
{
	TCHAR m_tmpBuffer[MAX_PATH] = { 0, };
	memset(m_tmpBuffer, 0, sizeof(TCHAR)*MAX_PATH);

	TCHAR* cPtr;
	_tcscpy(m_tmpBuffer, name);
	cPtr = m_tmpBuffer;
	// "MAP #1" -> MAP#1
	while (*cPtr)
	{
		if (*cPtr == '"')		*cPtr = SINGLE_QUOTE;
		else if (*cPtr == ' ' || *cPtr <= CTL_CHARS)
			*cPtr = _T('_');
		cPtr++;
	}
	return m_tmpBuffer;
}

void GbsWriter::GetTexture(Mtl* pMtl, GMtl& gSubMtl) {


	// 적용 가능한 텍스터 맵 개수만큼 순회한다.
	// 이때 현재 적용된 맵을 얻는다.-> 텍스처이름을 얻는다.
	int iNumMap = pMtl->NumSubTexmaps();
	for (int iMap = 0; iMap < iNumMap; iMap++)
	{
		Texmap* pTex = pMtl->GetSubTexmap(iMap);
		if (pTex == NULL) continue;
		//if (bitmap == NULL) continue;

		if (pTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
		{
			GTex tex;
			tex.iMapType = iMap;
			// stdmat.h
			BitmapTex* bitmap = (BitmapTex*)pTex;
			TSTR mapName = bitmap->GetMapName();
			SplitPathFile(mapName, NULL, &tex.szTexName);
			gSubMtl.texlist.push_back(tex);
		}
	}

}
void GbsWriter::AddObject(INode* pNode) {
	ObjectState os = pNode->EvalWorldState(m_Interval.Start());
	Object *obj = os.obj;
	if (obj)
	{
		switch (obj->SuperClassID()) 
		{
			case GEOMOBJECT_CLASS_ID:
			case HELPER_CLASS_ID:
			{
				m_ObjectList.push_back(pNode);
			}break;
		}
	}
}
void GbsWriter::AddMtl(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl != NULL) {
		bool bAdd = true;
		for (int iCnt = 0; iCnt < m_MtlList.size(); iCnt++) {
			if (m_MtlList[iCnt] == pMtl) {
				bAdd = false;
				break;
			}
		}
		if (bAdd)  m_MtlList.push_back(pMtl);
	}
	
}
void GbsWriter::PreProcess(INode* pNode) {
	if (pNode == NULL) return;

	AddObject(pNode);
	AddMtl(pNode);

	int iNumChildren = pNode->NumberOfChildren();
	for (int iChild = 0; iChild < iNumChildren; iChild++) {
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);

	}

}

void GbsWriter::Init(TSTR name,
	Interface* p3dsMax) {

	m_p3dsMax = p3dsMax;
	m_szFileName = name;
	m_pRootNode = m_p3dsMax->GetRootNode();
	m_Interval = m_p3dsMax->GetAnimRange();
	PreProcess(m_pRootNode);

	m_Scene.iVersion = 100;
	m_Scene.iNumMtl = m_MtlList.size();
	m_Scene.iNumObject = m_ObjectList.size();


	m_fp = _tfopen(m_szFileName, _T("wt"));
	_ftprintf(m_fp, _T("%s\n"), _T("GBS_FILE_EXPORT"));
};


bool GbsWriter::ExpScene() {
	
	//Interval m_Interval = m_p3dsMax->GetAnimRange();

	m_Scene.iTickPerFrame = GetTicksPerFrame();
	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();
	m_Scene.iFrameSpeed = GetFrameRate();
	_ftprintf(m_fp, _T("%s %d %d %d %d %d %d %d\n"),
		_T("#SCENE"),
		m_Scene.iVersion,
		m_Scene.iFirstFrame,
		m_Scene.iLastFrame,
		m_Scene.iFrameSpeed,
		m_Scene.iTickPerFrame,
		m_Scene.iNumMtl,
		m_Scene.iNumObject);	
	return true; 
};
bool GbsWriter::ExpMaterial() {
	
	for (int iMtl = 0; iMtl < m_MtlList.size(); iMtl++)
	{
		Mtl* pMtl = m_MtlList[iMtl];
		GMtl gMtl;

		gMtl.szName = FixupName(pMtl->GetName());

		if (pMtl->NumSubMtls() > 0)
		{
			for (int isubMtl = 0; isubMtl < pMtl->NumSubMtls(); isubMtl++)
			{
				Mtl* pSubMtl = pMtl->GetSubMtl(isubMtl);

				GMtl gSubMtl;
				GetTexture(pSubMtl, gSubMtl);
				gMtl.subMtls.push_back(gSubMtl);
			}
		}
		else
		{
			GetTexture(pMtl, gMtl);
		}
		m_gMtlList.push_back(gMtl);
	}

	for (int itMtl = 0; itMtl < m_gMtlList.size(); itMtl++)
	{
		_ftprintf(m_fp, _T("%s %d %d %d\n"), _T("#MATERIAL"),
			itMtl, m_gMtlList[itMtl].subMtls.size(),
			m_gMtlList[itMtl].texlist.size());

		if (m_gMtlList[itMtl].subMtls.size() > 0)
		{
			for (int isub = 0; isub < m_gMtlList[itMtl].subMtls.size(); isub++)
			{
				_ftprintf(m_fp, _T("\t%s %d %d %d\n"),
					_T("#SUBMATERIAL"), isub,
					m_gMtlList[itMtl].subMtls[isub].subMtls.size(),
					m_gMtlList[itMtl].subMtls[isub].texlist.size());

				for (int itex = 0; itex < m_gMtlList[itMtl].subMtls[isub].texlist.size(); itex++)
				{
					_ftprintf(m_fp, _T("\t\t%d %s\n"),
						m_gMtlList[itMtl].subMtls[isub].texlist[itex].iMapType,
						m_gMtlList[itMtl].subMtls[isub].texlist[itex].szTexName);
				}
			}
		}
		else
		{
			for (int itex = 0; itex < m_gMtlList[itMtl].texlist.size();
			itex++)
			{
				_ftprintf(m_fp, _T("%d %s\n"),
				m_gMtlList[itMtl].texlist[itex].iMapType,
				m_gMtlList[itMtl].texlist[itex].szTexName);
			}
		}
	}

	return true; 
};
bool GbsWriter::ExpObject() {

	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		GMesh  gMesh;
		INode* pINode = m_ObjectList[iObj];
		ObjectState os = pINode->EvalWorldState(m_Interval.Start());
		Object *obj = os.obj;
		if (obj)
		{
			switch (obj->SuperClassID())
			{
			case GEOMOBJECT_CLASS_ID:
			{
				gMesh.iType = 1;
			}break;
			case HELPER_CLASS_ID:
			{
				gMesh.iType = 0;
			}break;
			}
		}

		gMesh.nodeName = FixupName(pINode->GetName());
		INode* pParent = pINode->GetParentNode();
		gMesh.nodeParentName = FixupName(pParent->GetName());
		Matrix3  matWorld = pINode->GetNodeTM(m_Interval.Start());
		DumpMatrix3(matWorld, gMesh.matWorld);

		GetMesh(pINode, gMesh);
		GetAnimation(pINode, gMesh);

		Mtl* pMtl = pINode->GetMtl();
		for (int iMtl = 0; iMtl < m_MtlList.size(); iMtl++)
		{
			if (pMtl == m_MtlList[iMtl])
			{
				gMesh.iRef = iMtl;
				break;
			}
		}
		m_gObjectList.push_back(gMesh);
	}


	for (int iObj = 0; iObj < m_gObjectList.size(); iObj++)
	{
		_ftprintf(m_fp, _T("%s %d %s %s\n"),
			_T("#OBJECT"), iObj,
			m_gObjectList[iObj].nodeName,
			m_gObjectList[iObj].nodeParentName);

		_ftprintf(m_fp, _T("%s %10.4f %10.4f %10.4f %10.4f  %10.4f %10.4f %10.4f %10.4f  %10.4f %10.4f %10.4f %10.4f  %10.4f %10.4f %10.4f %10.4f \n"),
			_T("#WORLD"),
			m_gObjectList[iObj].matWorld._11,
			m_gObjectList[iObj].matWorld._12,
			m_gObjectList[iObj].matWorld._13,
			m_gObjectList[iObj].matWorld._14,

			m_gObjectList[iObj].matWorld._21,
			m_gObjectList[iObj].matWorld._22,
			m_gObjectList[iObj].matWorld._23,
			m_gObjectList[iObj].matWorld._24,

			m_gObjectList[iObj].matWorld._31,
			m_gObjectList[iObj].matWorld._32,
			m_gObjectList[iObj].matWorld._33,
			m_gObjectList[iObj].matWorld._34,

			m_gObjectList[iObj].matWorld._41,
			m_gObjectList[iObj].matWorld._42,
			m_gObjectList[iObj].matWorld._43,
			m_gObjectList[iObj].matWorld._44);


		std::sort(m_gObjectList[iObj].triList.begin(),
			m_gObjectList[iObj].triList.end(),
			AScendingSort());
		_ftprintf(m_fp, _T("%s %d\n"), _T("TRIANGLE"),
			m_gMtlList[m_gObjectList[iObj].iRef].subMtls.size());

		int iAddCount = 0;
		if (m_gMtlList[m_gObjectList[iObj].iRef].subMtls.size() >  0)
		{
			for (int iMtl = 0; iMtl <
				m_gMtlList[m_gObjectList[iObj].iRef].subMtls.size();
				iMtl++)
			{
				iAddCount += ExpMesh(m_gObjectList[iObj],
					iMtl,
					iAddCount);
			}
		}
		else
		{
			ExpMesh(m_gObjectList[iObj], -1, iAddCount);
		}


		/*	for (int iTri = 0; iTri < m_tObjectList[iObj].triList.size(); iTri++)
		{
		for (int iVertex = 0; iVertex < 3; iVertex++)
		{
		_ftprintf(m_fp,
		_T("%10.4f %10.4f %10.4f %10.4f  %10.4f %10.4f %10.4f %10.4f  %10.4f %10.4f %10.4f %10.4f \n"),
		m_tObjectList[iObj].triList[iTri].v[iVertex].p.x,
		m_tObjectList[iObj].triList[iTri].v[iVertex].p.y,
		m_tObjectList[iObj].triList[iTri].v[iVertex].p.z,

		m_tObjectList[iObj].triList[iTri].v[iVertex].n.x,
		m_tObjectList[iObj].triList[iTri].v[iVertex].n.y,
		m_tObjectList[iObj].triList[iTri].v[iVertex].n.z,

		m_tObjectList[iObj].triList[iTri].v[iVertex].c.x,
		m_tObjectList[iObj].triList[iTri].v[iVertex].c.y,
		m_tObjectList[iObj].triList[iTri].v[iVertex].c.z,
		m_tObjectList[iObj].triList[iTri].v[iVertex].c.w,

		m_tObjectList[iObj].triList[iTri].v[iVertex].t.x,
		m_tObjectList[iObj].triList[iTri].v[iVertex].t.y );
		}
		}*/

	}

	return true; 
};
bool GbsWriter::Exporter() {

	ExpScene();
	ExpMaterial();
	ExpObject();
	return true;
};
GbsWriter::GbsWriter()
{
}


GbsWriter::~GbsWriter()
{
}
