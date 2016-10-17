#pragma once
#include "GbsMaterial.h"

struct TTCBKey 
{
	DWORD flags;
	float tens;		// Tension
	float cont;		// Continuity
	float bias;		// Bias
	float easeIn;
	float easeOut;
};
struct TAnimTrack	: public TTCBKey
{
	DWORD			iType;
	int				iTick;
	Quat			qValue;
	Point3			vValue;
	Point3			intan;		//Inconming Tangent Vector
	Point3			outtan;		//Outgoing Tanglent Vector
	Point3			inLength;	//The in length of the horizontal handle
	Point3			outLength;	//The out length of the horizontal handle
};
struct TMESH_HEADER
{
	OBJECTCLASSTYPE		m_ClassType;
	Box3				m_Box;
	D3D_MATRIX			m_matWorld;
	int					m_iNumVertex;
	int					m_iNumFace;
	int					m_iMtrlRef;
};

struct TMesh : public TMESH_HEADER
{	
	TSTR				m_strNodeName;
	TSTR				m_strParentName;	
	vector<TAnimTrack>	m_PosTrack;
	vector<TAnimTrack>	m_RotTrack;
	vector<TAnimTrack>	m_SclTrack;
	vector<TAnimTrack>	m_VisTrack;
	vector<TTri>		m_TriList;	
	vector<TMesh>		m_pSubMesh;

	TMesh() 
	{	
		m_iMtrlRef = -1;
		m_iNumFace = 0;
		m_ClassType = CLASS_GEOM;		
	};
	~TMesh()
	{
		m_PosTrack.clear();
		m_RotTrack.clear();
		m_SclTrack.clear();
		m_VisTrack.clear();
		m_TriList.clear();
	};
};

class TbsObject : public TbsMaterial
{
public:
	vector<TMesh>					m_Mesh;	// 출력 할 정보 리스트
public:
	void	UpdateObject();
	void	GetObject( MATRIXINDEX* pPoint, int iObj  );//오브젝트 리스트에서 정보 얻기
	// 오브젝트 정보 
	void	GetNodeHeader(INode* node, TMesh&	pMesh );
	void	GetNodeTM(INode* node, TMesh&	pMesh);	
	// 메쉬 정보
	void	GetMesh(INode* node, TMesh&	pMesh );	
	Point3	GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	TriObject* GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);
	//에니메이션
	virtual void	GetAnimKeys( INode*, TMesh& ){};
public:
	TbsObject(void);
	virtual ~TbsObject(void);
};
