#pragma once
#include "GbsObject.h"

//struct TTCBKey 
//{
//	DWORD flags;
//	float tens;		// Tension
//	float cont;		// Continuity
//	float bias;		// Bias
//	float easeIn;
//	float easeOut;
//};
//struct TAnimTrack	: public TTCBKey
//{
//	DWORD			m_dwType;
//	int				m_iTick;
//	Quat			qValue;
//	Point3			m_vValue;
//
//	Point3			m_vIntan;		//Inconming Tangent Vector
//	Point3			m_vOuttan;		//Outgoing Tanglent Vector
//	Point3			m_inLength;	//The in length of the horizontal handle
//	Point3			m_outLength;	//The out length of the horizontal handle
//
//	TAnimTrack*		m_pNext;
//	TAnimTrack*		m_pPrev;	
//	TAnimTrack() : m_pNext(0), m_pPrev(0) {}
//};

class TbsAnimObj:public TbsObject
{
public:
	int		m_iDeltaTick;
	// 샘플링
	void	GetAnimKeys( INode* pNode, TMesh* pMesh);
	void	DumpScaleSample( INode* node, TMesh* Mesh ); 
	void	DumpRotSample( INode* node, TMesh* Mesh );
	void	DumpPosSample(INode* node, TMesh* Mesh );
	BOOL	CheckForAnimation(INode* node, BOOL& bPos, BOOL& bRot, BOOL& bScale);
	bool	GetDecompAffine( TimeValue t, INode* pNode, AffineParts* ap, 
							 Point3* rotAxis=NULL, float* rotAngle=NULL  );
	// 키 프레임 
	void	DumpFloatKeys(Control* cont, TMesh* pMesh); 
	//void	DumpPosKeys(Control* cont, TMesh* pMesh);
	//void	DumpRotKeys(Control* cont, TMesh* pMesh);
	//void	DumpScaleKeys(Control* cont, TMesh* pMesh) ;
	//BOOL	IsKnownController(Control* cont);	

public:
	TbsAnimObj(void);
	virtual ~TbsAnimObj(void);
};
