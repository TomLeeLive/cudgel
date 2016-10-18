#include "GbsWriter.h"

struct DescendingDateSort
{
	bool operator()(TTri& rpStart, TTri& rpEnd)
	{		
		return rpStart.iSubIndex < rpEnd.iSubIndex;
	}
};
static int g_iSearchIndex = 0;
struct IsSameInt // find_to와 같은지 판단해 주는 함수자   
{   
    bool operator()(TTri &value)   
    {   
        return value.iSubIndex == g_iSearchIndex;   
    }   
}; 

bool TbsWriter::Init(TSTR strFileName, Interface* pMax )
{
	m_strFileName = strFileName;	
	return Initialize( pMax );
}

bool TbsWriter::Export()
{
	// 실제 정보를 저장한다.
	GetMaterial();
	UpdateObject();

	// 저장된 정보를 출력한다.
	m_pStream = _tfopen( m_strFileName,_T("wt") );
	if ( !m_pStream ) 
	{
		return 0;
	}	
	ExpScene();
	ExpMaterial();
	ExpObject();	// Object	

	fclose(m_pStream);
	MessageBox( GetActiveWindow(), m_strFileName, _T("Succeed!!"), MB_OK );
	return true;
}

bool TbsWriter::Release()
{
	return true;
}
void TbsWriter::ExpScene()
{
	struct tm *newtime;
	time_t aclock;

	time( &aclock );
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);	// The date string has a \n appended.
	today.remove(today.length()-1);		// Remove the \n

	_ftprintf( m_pStream, _T("%s %s\n"), _T("#TBF_FILE_EXPORT") , FixupName(today) );
	_ftprintf( m_pStream, _T("%s\n"), _T("#SCENE"));	// Scene
	_ftprintf( m_pStream, _T("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d"),
											m_Scene.iVersion,
											m_Scene.iFirstFrame,
											m_Scene.iLastFrame,
											m_Scene.iFrameSpeed,
											m_Scene.iTickPerFrame,
											m_Scene.iNumMesh,
											m_Scene.iMaxWeight,
											m_Scene.iBindPose);
}
void TbsWriter::ExpMaterial()
{
	// SubMaterial count
	_ftprintf( m_pStream, _T("\n%s\t%d"), _T("#ROOT_MATERIAL"),m_Material.size() );
	// Material	
	for( int iRootMtl = 0; iRootMtl < m_Material.size(); iRootMtl++ )
	{		
		//! RootMaterial Of Index
		_ftprintf( m_pStream, _T("\n%s %d\n"), _T("#MATERIAL"), m_Material[iRootMtl].iIndex );
		// CurentMaterial Name
		if( !m_Material[iRootMtl].strName.isNull() )
			_ftprintf( m_pStream, _T("\t%s"), m_Material[iRootMtl].strName);
		else
			_ftprintf( m_pStream, _T("\t%s"), _T("NULL") );

		// Class Name
		if( !m_Material[iRootMtl].strClassName.isNull() )
			_ftprintf( m_pStream, _T("\t%s"), m_Material[iRootMtl].strClassName);
		else
			_ftprintf( m_pStream, _T("\t%s"), _T("NULL") );

		// SubMaterial count
		_ftprintf( m_pStream, _T("\t%d"), m_Material[iRootMtl].iNumSubMtl );

		if( m_Material[iRootMtl].iNumSubMtl > 0 )
		{
			// TexMapCount Of RootMtrl 
			_ftprintf( m_pStream, _T("\t%d"), m_Material[iRootMtl].TexMaps.size() );

			for( int iSubMtl = 0; iSubMtl < m_Material[iRootMtl].iNumSubMtl; iSubMtl++ )
			{	// SubMaterial Index
				_ftprintf( m_pStream, _T("\n\t%s %d"), _T("#SUBMATERIAL"), m_Material[iRootMtl].SubMaterial[iSubMtl].iIndex );
				if( !m_Material[iRootMtl].SubMaterial[iSubMtl].strName.isNull() )
					_ftprintf( m_pStream, _T("\n\t\t%s"), m_Material[iRootMtl].SubMaterial[iSubMtl].strName);
				else
					_ftprintf( m_pStream, _T("\t%s"), _T("NULL") );

				if( !m_Material[iRootMtl].SubMaterial[iSubMtl].strClassName.isNull() )
					_ftprintf( m_pStream, _T("\t%s"), m_Material[iRootMtl].SubMaterial[iSubMtl].strClassName);
				else
					_ftprintf( m_pStream, _T("\t%s"), _T("NULL") );
				// Not! Support..  Sub->Sub Material 
				_ftprintf( m_pStream, _T("\t%d"),	m_Material[iRootMtl].SubMaterial[iSubMtl].iNumSubMtl );

				ExpTexture(m_Material[iRootMtl].SubMaterial[iSubMtl].TexMaps );				
			}
		}
		else
		{
			ExpTexture(m_Material[iRootMtl].TexMaps);			
		}		
	}
}
void TbsWriter::ExpTexture( vector<TTexMap>& TexMap )
{	
	_ftprintf( m_pStream, _T("\t%d"), TexMap.size() );
	for( int iTexMap = 0; iTexMap < TexMap.size(); iTexMap++ )
	{
		if( !TexMap[iTexMap].strTextureName.isNull() )
		{
			_ftprintf( m_pStream, _T("\n\t\t%d\t%s"),	TexMap[iTexMap].iType,
												TexMap[iTexMap].strTextureName );
		}
		else
		{
			_ftprintf( m_pStream, _T("\t\t%d\t%s"),	TexMap[iTexMap].iType,
												_T("NULL"));
		}

	}
}
int TbsWriter::IsEqulVertexList( vector<PNCT_VERTEX>&  VertexArray, PNCT_VERTEX& Vertex )
{
	for( int iVertex=0; iVertex < VertexArray.size(); iVertex++ ) 
	{
		if( EqualPoint3( VertexArray[iVertex].p, Vertex.p ) &&
			EqualPoint3( VertexArray[iVertex].n, Vertex.n ) &&
			EqualPoint4( VertexArray[iVertex].c, Vertex.c ) &&
			EqualPoint2( VertexArray[iVertex].t, Vertex.t )  )					
		{				
			return iVertex;
		}
	}
	return -1;
}

int TbsWriter::SetUniqueBuffer( TMesh* pMesh, int iMtrl, int iStartTri, vector <PNCT_VERTEX>& VertexArray, vector<int>& IndexArray )
{
	int iNumFaces = pMesh->m_TriList.size();
	if( iMtrl >= 0 )
	{
		g_iSearchIndex = iMtrl;	
		iNumFaces = count_if( pMesh->m_TriList.begin(), pMesh->m_TriList.end(), IsSameInt() );
	}

	int iPosPushCnt = 0;
	for( int iFace = 0; iFace < iNumFaces; iFace++ )
	{			
		for( int iCnt = 0; iCnt < 3; iCnt++ )
		{
			int iPosReturn = IsEqulVertexList( VertexArray, pMesh->m_TriList[iStartTri+iFace].v[iCnt] );
			if( iPosReturn < 0 )
			{				
				VertexArray.push_back(pMesh->m_TriList[iStartTri+iFace].v[iCnt]);
				iPosReturn = iPosPushCnt++;
			}
			IndexArray.push_back( iPosReturn );			
		}				
	}
	return iNumFaces;
}

void TbsWriter::ExpObject()
{
	for( int iMesh = 0; iMesh < m_Mesh.size(); iMesh++ )
	{	
		TMesh* pMesh = &m_Mesh[iMesh];		
		
		_ftprintf( m_pStream, _T("\n%s %d"), _T("#OBJECTS"), iMesh );
		_ftprintf( m_pStream, _T("\n%s %s"), pMesh->m_strNodeName, pMesh->m_strParentName  );
		_ftprintf( m_pStream, _T("\n%d\t%d\t%d\t%d\t%d\t%d\t%d\n"),
												pMesh->m_ClassType,
												pMesh->m_iNumFace, 
												pMesh->m_PosTrack.size(),
												pMesh->m_RotTrack.size(),
												pMesh->m_SclTrack.size(),
												pMesh->m_VisTrack.size(),										
												pMesh->m_iMtrlRef );

		_ftprintf( m_pStream, _T("\t%f %f %f\t%f %f %f\n"),	pMesh->m_Box.pmax.x,
												pMesh->m_Box.pmax.y,
												pMesh->m_Box.pmax.z,
												pMesh->m_Box.pmin.x,
												pMesh->m_Box.pmin.y,
												pMesh->m_Box.pmin.z );		

		_ftprintf( m_pStream, _T("\t%s\n"), _T("#WORLD_MATRIX"));
		_ftprintf( m_pStream, _T("\t%10.4f %10.4f %10.4f %10.4f\n"),
		pMesh->m_matWorld._11,pMesh->m_matWorld._12,pMesh->m_matWorld._13,pMesh->m_matWorld._14 );		
		_ftprintf( m_pStream, _T("\t%10.4f %10.4f %10.4f %10.4f\n"),
		pMesh->m_matWorld._21,pMesh->m_matWorld._22,pMesh->m_matWorld._23,pMesh->m_matWorld._24);			
		_ftprintf( m_pStream, _T("\t%10.4f %10.4f %10.4f %10.4f\n"),
		pMesh->m_matWorld._31,pMesh->m_matWorld._32,pMesh->m_matWorld._33,pMesh->m_matWorld._34);			
		_ftprintf( m_pStream, _T("\t%10.4f %10.4f %10.4f %10.4f\n"),
		pMesh->m_matWorld._41,pMesh->m_matWorld._42,pMesh->m_matWorld._43,pMesh->m_matWorld._44 );

	
		_ftprintf( m_pStream, _T("\t%s\n"), _T("#TRAINGLES"));
		std::sort( pMesh->m_TriList.begin(), pMesh->m_TriList.end(), DescendingDateSort());

		int iAddCount = 0;
		// 서브매터리얼을 사용할 경우
		if( pMesh->m_iMtrlRef >= 0 && m_Material[pMesh->m_iMtrlRef].iNumSubMtl > 0)
		{
			for( int iMtrl = 0; iMtrl < m_Material[pMesh->m_iMtrlRef].iNumSubMtl; iMtrl++ )
			{			
				iAddCount += ExpMesh(m_pStream, pMesh, iMtrl,iAddCount );					
			}			
		}
		else
		{
			ExpMesh(m_pStream, pMesh, -1, 0 );		
		}

		// 에니메이션 정보 출력		
		ExpAnimation(pMesh);
	}
}
int TbsWriter::ExpMesh(FILE* fp, TMesh* pMesh, int iMtrl, int iAddCount )
{
	vector <PNCT_VERTEX>			VertexArray;
	vector <int>					IndexArray;
	int iNumFaces = SetUniqueBuffer( pMesh, iMtrl, iAddCount, VertexArray, IndexArray );

	_ftprintf( fp, _T("\t%d \t%d %d\n"), iMtrl,	iNumFaces, VertexArray.size() );

	for( int iCnt = 0; iCnt < VertexArray.size(); iCnt++ )
	{
			_ftprintf( fp, _T("\t%10.4f %10.4f %10.4f\t%10.4f %10.4f %10.4f \t %10.4f %10.4f %10.4f %10.4f \t%10.4f %10.4f\n"),
					VertexArray[iCnt].p.x,VertexArray[iCnt].p.y,VertexArray[iCnt].p.z,
					VertexArray[iCnt].n.x,VertexArray[iCnt].n.y,VertexArray[iCnt].n.z,
					VertexArray[iCnt].c.x,VertexArray[iCnt].c.y,VertexArray[iCnt].c.z,VertexArray[iCnt].c.w,
					VertexArray[iCnt].t.x,VertexArray[iCnt].t.y );								
	}

	for( int iIndex = 0; iIndex < IndexArray.size(); iIndex+=3 )
	{			
		_ftprintf( fp, _T("\t%d %d %d\n"),	IndexArray[iIndex+0],IndexArray[iIndex+1],IndexArray[iIndex+2] );
	}
	return iNumFaces;
}
void TbsWriter::ExpAnimation(TMesh* pMesh)
{
	if( pMesh->m_PosTrack.size() > 0 )
	{
		_ftprintf( m_pStream, _T("\t%s\n"), _T("#POSITION_TRACK"));
		for( DWORD dwPosTrack = 0; dwPosTrack < pMesh->m_PosTrack.size(); dwPosTrack++ )
		{
			_ftprintf( m_pStream, _T("\t%d %10.4f %10.4f %10.4f\n"),
												pMesh->m_PosTrack[dwPosTrack].iTick,
												pMesh->m_PosTrack[dwPosTrack].vValue.x,
												pMesh->m_PosTrack[dwPosTrack].vValue.y,
												pMesh->m_PosTrack[dwPosTrack].vValue.z );
		}
	}
	if( pMesh->m_RotTrack.size() > 0 )
	{
		_ftprintf( m_pStream, _T("\t%s\n"), _T("#ROTATION_TRACK"));
		for( DWORD dwRotTrack = 0; dwRotTrack < pMesh->m_RotTrack.size(); dwRotTrack++ )
		{
			_ftprintf( m_pStream, _T("\t%d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f\n"),
													pMesh->m_RotTrack[dwRotTrack].iTick,
													pMesh->m_RotTrack[dwRotTrack].vValue.x,
													pMesh->m_RotTrack[dwRotTrack].vValue.y,
													pMesh->m_RotTrack[dwRotTrack].vValue.z,
													pMesh->m_RotTrack[dwRotTrack].qValue.x,
													pMesh->m_RotTrack[dwRotTrack].qValue.y,
													pMesh->m_RotTrack[dwRotTrack].qValue.z,
													pMesh->m_RotTrack[dwRotTrack].qValue.w );
		}
	}
	if( pMesh->m_SclTrack.size() > 0)
	{
		_ftprintf( m_pStream, _T("\t%s\n"), _T("#SCALE_TRACK"));
		for( DWORD dwSclTrack = 0; dwSclTrack < pMesh->m_SclTrack.size(); dwSclTrack++ )
		{
			_ftprintf( m_pStream, _T("\t%d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f\n"),
															pMesh->m_SclTrack[dwSclTrack].iTick,			
															pMesh->m_SclTrack[dwSclTrack].vValue.x,
															pMesh->m_SclTrack[dwSclTrack].vValue.y,
															pMesh->m_SclTrack[dwSclTrack].vValue.z,
															pMesh->m_SclTrack[dwSclTrack].qValue.x,
															pMesh->m_SclTrack[dwSclTrack].qValue.y,
															pMesh->m_SclTrack[dwSclTrack].qValue.z,
															pMesh->m_SclTrack[dwSclTrack].qValue.w );
		}
	}
	if( pMesh->m_VisTrack.size() > 0 )
	{
		_ftprintf( m_pStream, _T("\t%s\n"), _T("#VISIVILITY_TRACK"));
		for( DWORD dwVisTrack = 0; dwVisTrack < pMesh->m_VisTrack.size(); dwVisTrack++ )
		{
			_ftprintf( m_pStream, _T("\t%d %10.4f\n"),	pMesh->m_VisTrack[dwVisTrack].iTick,
													pMesh->m_VisTrack[dwVisTrack].vValue.x );
		}
	}
}
TbsWriter::TbsWriter(void)
{
}

TbsWriter::~TbsWriter(void)
{
}
