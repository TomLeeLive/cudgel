#pragma once
#include "GbsAnimObj.h"

class TbsWriter : public TbsAnimObj
{
	public:
		FILE*		m_pStream;	// 출력 파일 포인터
		TSTR		m_strFileName;	// 출력 할 파일명
	public:
		void	PreProcess( INode* pNode ); // INode 순회
		bool 	Init(TSTR strFileName, Interface* pMax ); // 초기화
		bool	Export();		// 파일 출력
		bool	Release();		// 메모리 소멸 및 해제	
		//출력
	public:
		void	ExpScene();
		void	ExpMaterial();
		void	ExpObject();
		int		ExpMesh(FILE* fp, TMesh* pMesh,  int iMtrl=-1, int iAddCount=0 );
		void	ExpAnimation(TMesh* pMesh);
		void	ExpTexture( vector<TTexMap>& TexMap );
		int		IsEqulVertexList( vector<PNCT_VERTEX>&  VertexArray, PNCT_VERTEX& Vertex );
		int		SetUniqueBuffer( TMesh* pMesh, int iMtrl, int iStartTri, vector <PNCT_VERTEX>& VertexArray, vector<int>& IndexArray );

	public:
		TbsWriter ();
		virtual~ TbsWriter ();
};

