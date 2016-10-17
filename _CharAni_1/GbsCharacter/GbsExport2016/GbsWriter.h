#pragma once
#include "GbsAnimObj.h"

class TbsWriter : public TbsAnimObj
{
	public:
		FILE*		m_pStream;	// ��� ���� ������
		TSTR		m_strFileName;	// ��� �� ���ϸ�
	public:
		void	PreProcess( INode* pNode ); // INode ��ȸ
		bool 	Init(TSTR strFileName, Interface* pMax ); // �ʱ�ȭ
		bool	Export();		// ���� ���
		bool	Release();		// �޸� �Ҹ� �� ����	
		//���
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

