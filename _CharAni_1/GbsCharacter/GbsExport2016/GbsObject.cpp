#include "GbsObject.h"

void TbsObject::UpdateObject()
{
	m_Mesh.resize(m_MatrixMap.Count());
	for( int iObj = 0;iObj < m_MatrixMap.Count(); iObj++ ) 
	{
		MATRIXINDEX *pPoint = (MATRIXINDEX *)m_MatrixMap.TMap[iObj];			
		GetObject( pPoint, iObj );
	}
}

void TbsObject::GetObject( MATRIXINDEX* pPoint, int iObj )
{
	m_Mesh[iObj].m_ClassType = pPoint->m_ClassType;
	GetNodeHeader( pPoint->m_pINode, m_Mesh[iObj] );	// 노드의 이름을 얻는다.
	GetNodeTM( pPoint->m_pINode, m_Mesh[iObj] );	// 노드의 행렬을 얻는다.
	if( pPoint->m_ClassType == CLASS_GEOM )
	{
		GetMesh( pPoint->m_pINode, m_Mesh[iObj]);	// 기하 정보를 얻는다.		
	}else
	{
		Object* helperObj = pPoint->m_pObj;				
		Matrix3		oTM = pPoint->m_pINode->GetObjectTM(m_Interval.Start());							
		helperObj->GetDeformBBox(m_Interval.Start(), m_Mesh[iObj].m_Box, &oTM);
		INode* pParent = pPoint->m_pINode->GetParentNode();
		m_Mesh[iObj].m_Box.pmax = m_Mesh[iObj].m_Box.pmax * Inverse(pParent->GetNodeTM(m_Interval.Start()));
		m_Mesh[iObj].m_Box.pmin = m_Mesh[iObj].m_Box.pmin * Inverse(pParent->GetNodeTM(m_Interval.Start()));
	}
	GetAnimKeys( pPoint->m_pINode, m_Mesh[iObj] );	// 에니메이션 정보를 얻는다.
}

void TbsObject::GetNodeHeader(INode* node, TMesh&	tMesh )
{	
	tMesh.m_strNodeName = FixupName( node->GetName() );	
	INode* parent = node->GetParentNode();
	if( parent && ( !parent->IsRootNode() ) ) 
	{
		tMesh.m_strParentName = FixupName ( parent->GetName());			
	}
}
void TbsObject::GetNodeTM(INode* node, TMesh&	tMesh)
{	
	Matrix3 g_matWorld = node->GetNodeTM(m_Interval.Start());			
	DumpMatrix3(&g_matWorld, tMesh.m_matWorld );		
}

void TbsObject::GetMesh(INode* node, TMesh&	tMesh )
{	
	Matrix3 tm = node->GetObjTMAfterWSM(m_Interval.Start());	
	// 작업중 및 작업완료된 파이프라인의 결과에 액세스함으로써 현재노드의 결과를 돌려준다.
	ObjectState os = node->EvalWorldState(m_Interval.Start());
	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) 
	{
		return; 
	}	
	
	BOOL needDel;
	TriObject* tri = GetTriObjectFromNode(node,m_Interval.Start(), needDel);
	if (!tri) 
	{
		return;
	}
	
	Mesh* mesh = &(tri->GetMesh());	
	// 바운딩 박스
	mesh->buildBoundingBox();
	Box3 box = mesh->getBoundingBox(&tm);
	DumpPoint3(tMesh.m_Box.pmax, box.pmax );
	DumpPoint3(tMesh.m_Box.pmin, box.pmin );

	tMesh.m_iNumFace		= mesh->getNumFaces();

	BOOL negScale = TMNegParity(tm);
	int v0, v1, v2;
	if (negScale) {		v0 = 2;		v1 = 1;		v2 = 0;	}
	else {				v0 = 0;		v1 = 1;		v2 = 2;	}

	tMesh.m_TriList.resize(tMesh.m_iNumFace);
	for( int iFace=0; iFace < tMesh.m_iNumFace; iFace++) 
	{
		Matrix3 pInverse = Inverse(node->GetNodeTM(m_Interval.Start()));
		Point3 v;		
		if( mesh->getNumVerts() > 0 )
		{
/*			vv = tm * mesh->verts[   mesh->faces[iFace].v[v0]    ];
			v = mesh->verts[   mesh->faces[iFace].v[v0]    ] * tm;	*/	
			v = mesh->verts[ mesh->faces[iFace].v[v0]] * tm * pInverse;			
			DumpPoint3(tMesh.m_TriList[iFace].v[0].p, v );

			v = mesh->verts[ mesh->faces[iFace].v[v2]] * tm * pInverse;
			DumpPoint3(tMesh.m_TriList[iFace].v[1].p, v );

			v = mesh->verts[ mesh->faces[iFace].v[v1]] * tm * pInverse;
			DumpPoint3(tMesh.m_TriList[iFace].v[2].p, v );
		}
		if( mesh->getNumTVerts() > 0 )
		{
			tMesh.m_TriList[iFace].v[0].t.x		= mesh->tVerts[mesh->tvFace[iFace].t[v0]].x;
			tMesh.m_TriList[iFace].v[0].t.y		= 1.0f - mesh->tVerts[mesh->tvFace[iFace].t[v0]].y;
			tMesh.m_TriList[iFace].v[1].t.x		= mesh->tVerts[mesh->tvFace[iFace].t[v2]].x;
			tMesh.m_TriList[iFace].v[1].t.y		= 1.0f - mesh->tVerts[mesh->tvFace[iFace].t[v2]].y;
			tMesh.m_TriList[iFace].v[2].t.x		= mesh->tVerts[mesh->tvFace[iFace].t[v1]].x;
			tMesh.m_TriList[iFace].v[2].t.y		= 1.0f - mesh->tVerts[mesh->tvFace[iFace].t[v1]].y;
		}
		
		tMesh.m_TriList[iFace].v[0].c  = Point4( 1,1,1,1 );
		tMesh.m_TriList[iFace].v[1].c  = Point4( 1,1,1,1 );
		tMesh.m_TriList[iFace].v[2].c  = Point4( 1,1,1,1 );

		if( mesh->getNumVertCol() > 0 )
		{
			tMesh.m_TriList[iFace].v[0].c.x = mesh->vertCol[mesh->vcFace[iFace].t[v0]].x;
			tMesh.m_TriList[iFace].v[0].c.y = mesh->vertCol[mesh->vcFace[iFace].t[v0]].y;
			tMesh.m_TriList[iFace].v[0].c.z = mesh->vertCol[mesh->vcFace[iFace].t[v0]].z;
			tMesh.m_TriList[iFace].v[0].c.w = 1.0f;
			tMesh.m_TriList[iFace].v[1].c.x = mesh->vertCol[mesh->vcFace[iFace].t[v2]].x;
			tMesh.m_TriList[iFace].v[1].c.y = mesh->vertCol[mesh->vcFace[iFace].t[v2]].y;
			tMesh.m_TriList[iFace].v[1].c.z = mesh->vertCol[mesh->vcFace[iFace].t[v2]].z;
			tMesh.m_TriList[iFace].v[1].c.w = 1.0f;
			tMesh.m_TriList[iFace].v[2].c.x = mesh->vertCol[mesh->vcFace[iFace].t[v1]].x;
			tMesh.m_TriList[iFace].v[2].c.y = mesh->vertCol[mesh->vcFace[iFace].t[v1]].y;
			tMesh.m_TriList[iFace].v[2].c.z = mesh->vertCol[mesh->vcFace[iFace].t[v1]].z;
			tMesh.m_TriList[iFace].v[2].c.w = 1.0f;
		}	
		
		// 정점 노말 얻기
		mesh->buildNormals();	

		int  vert	= mesh->faces[iFace].getVert(v0);
		Point3 vn	= GetVertexNormal(mesh, iFace, mesh->getRVertPtr(vert));	
		DumpPoint3(tMesh.m_TriList[iFace].v[0].n, vn );

		vert	= mesh->faces[iFace].getVert(v2);
		vn		= GetVertexNormal(mesh, iFace, mesh->getRVertPtr(vert));	
		DumpPoint3(tMesh.m_TriList[iFace].v[1].n, vn );

		vert	= mesh->faces[iFace].getVert(v1);
		vn		= GetVertexNormal(mesh, iFace, mesh->getRVertPtr(vert));	
		DumpPoint3(tMesh.m_TriList[iFace].v[2].n, vn );
		
		// 서브 매터리얼 인덱스
		tMesh.m_TriList[iFace].iSubIndex = mesh->faces[iFace].getMatID();	
	}
	if (needDel) 
	{
		delete tri;
	}	
	tMesh.m_iMtrlRef = GetMtlRef( node->GetMtl() );
}

Point3 TbsObject::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals = rv->rFlags & NORCT_MASK;
	Point3 vertexNormal;	
	
	if (rv->rFlags & SPECIFIED_NORMAL) 
	{
		vertexNormal = rv->rn.getNormal();
	}	
	else if ( numNormals  && smGroup) 
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
	else 
	{	
		vertexNormal = mesh->getFaceNormal(faceNo);
	}	
	return vertexNormal;
}
TriObject* TbsObject::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) 
	{ 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));		
		if (obj != (Object*)tri) deleteIt = TRUE;
			return tri;
	}
	else 
	{
		return NULL;
	}
}
TbsObject::TbsObject(void)
{

}

TbsObject::~TbsObject(void)
{
}
