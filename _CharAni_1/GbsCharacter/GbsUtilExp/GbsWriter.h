#pragma once
#include "GbsAnimObj.h"

class TbsWriter : public TbsAnimObj, public TSingleton< TbsWriter >
{
private:
	friend class TSingleton<TbsWriter>;
	public:
		FILE*		m_pStream;	// ��� ���� ������
		TSTR		m_strFileName;	// ��� �� ���ϸ�
	public:
		bool 	Init(TSTR strFileName, Interface* pMax ); // �ʱ�ȭ
		bool	tbsExport();		// ���� ���
		bool	Release();		// �޸� �Ҹ� �� ����	
		//��� ���� ���� ��ȭ����
		TCHAR*	SaveFileDlg(TCHAR* szExt, TCHAR* szTitle=_T("TBasisExporter"));
		void	ExpScene();
		void	ExpMaterial();
		void	ExpObject();
		int		ExpMesh(FILE* fp, TMesh* pMesh,  int iMtrl=-1, int iAddCount=0 );
		void	ExpAnimation(TMesh* pMesh);
		void	ExpTexture( vector<TTexMap>& TexMap );
		int		IsEqulVertexList( vector<PNCT_VERTEX>&  VertexArray, PNCT_VERTEX& Vertex );
		int		SetUniqueBuffer( TMesh* pMesh, int iMtrl, int iStartTri, vector <PNCT_VERTEX>& VertexArray, vector<int>& IndexArray );
		void	matExport();
	public:
		TbsWriter ();
		virtual ~TbsWriter ();
};

#define I_Writer TbsWriter::GetInstance()

