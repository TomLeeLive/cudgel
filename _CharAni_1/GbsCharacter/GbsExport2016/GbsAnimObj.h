#pragma once
#include "GbsObject.h"

class TbsAnimObj:public TbsObject
{
public:
	int		m_iDeltaTick;
	// 샘플링
	void	GetAnimKeys( INode* pNode, TMesh& pMesh);
	void	DumpScaleSample( INode* node, TMesh& Mesh ); 
	void	DumpRotSample( INode* node, TMesh& Mesh );
	void	DumpPosSample(INode* node, TMesh& Mesh );
	BOOL	CheckForAnimation(INode* node, BOOL& bPos, BOOL& bRot, BOOL& bScale);
	bool	GetDecompAffine( TimeValue t, INode* pNode, AffineParts* ap, 
							 Point3* rotAxis=NULL, float* rotAngle=NULL  );
	// 키 프레임 
	void	DumpFloatKeys(Control* cont, TMesh& pMesh); 
	void	DumpPosKeys(Control* cont, TMesh& pMesh);
	void	DumpRotKeys(Control* cont, TMesh& pMesh);
	void	DumpScaleKeys(Control* cont, TMesh& pMesh) ;
	BOOL	IsKnownController(Control* cont);	

public:
	TbsAnimObj(void);
	virtual ~TbsAnimObj(void);
};
