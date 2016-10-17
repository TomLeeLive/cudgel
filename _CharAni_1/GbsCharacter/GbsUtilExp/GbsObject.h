#pragma once
#include "GbsGlobal.h"

class TbsObject : public TbsGlobal
{
public:
	void	GetMaterial();//���͸��� ����Ʈ���� ���� ���
	void	GetTexture(TMtl* pDestMtl, Mtl* pSrcMtl);//���͸��� ����Ʈ���� ���� ���
public:
	void	UpdateObject();
	void	GetObject( MATRIXINDEX* pPoint  );//������Ʈ ����Ʈ���� ���� ���
	// ������Ʈ ���� 
	void	GetNodeHeader(INode* node, TMesh*	pMesh );
	void	GetNodeTM(INode* node, TMesh*	pMesh);	
	// �޽� ����
	void	GetMesh(INode* node, TMesh*	pMesh );	
	Point3	GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	TriObject* GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);
	//���ϸ��̼�
	virtual void	GetAnimKeys( INode*, TMesh* ){};
public:
	TbsObject(void);
	virtual ~TbsObject(void);
};
