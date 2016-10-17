#pragma once
#include "GbsGlobal.h"

class TbsObject : public TbsGlobal
{
public:
	void	GetMaterial();//매터리얼 리스트에서 정보 얻기
	void	GetTexture(TMtl* pDestMtl, Mtl* pSrcMtl);//매터리얼 리스트에서 정보 얻기
public:
	void	UpdateObject();
	void	GetObject( MATRIXINDEX* pPoint  );//오브젝트 리스트에서 정보 얻기
	// 오브젝트 정보 
	void	GetNodeHeader(INode* node, TMesh*	pMesh );
	void	GetNodeTM(INode* node, TMesh*	pMesh);	
	// 메쉬 정보
	void	GetMesh(INode* node, TMesh*	pMesh );	
	Point3	GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	TriObject* GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);
	//에니메이션
	virtual void	GetAnimKeys( INode*, TMesh* ){};
public:
	TbsObject(void);
	virtual ~TbsObject(void);
};
