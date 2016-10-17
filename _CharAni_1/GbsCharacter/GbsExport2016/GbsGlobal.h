#pragma once
#include "GbsExport2016.h"
#include "GTemplateMap.h"

#define CTL_CHARS		31
#define SINGLE_QUOTE	39 // ( ' )
#define ALMOST_ZERO		1.0e-3f


#ifdef __cplusplus
typedef struct _D3D_MATRIX {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;
        };
        float m[4][4];
    };
}D3D_MATRIX, *LPD3D_MATRIX;
#else
	typedef struct _D3D_MATRIX D3D_MATRIX, *LPD3D_MATRIX;
#endif

typedef class Matrix3		*LPMATRIX3;

// ������������������������������������������������������������������
//  �����е� �޽�
// ������������������������������������������������������������������
struct P3NCVERTEX
{	
	float		Px,Py,Pz;	
	float		Nx, Ny, Nz;
	Point4		c;	
};
struct PNCT_VERTEX
{
	Point3		p;
	Point3		n;
	Point4		c;
    Point2	    t;
};

const enum OBJECTCLASSTYPE {
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY, 
	CLASS_BIPED,
};
struct TTri
{
	PNCT_VERTEX	v[3];
	int			iSubIndex;
};
struct TScene
{
	int iVersion;		// ���� ����
	int	iFirstFrame;    // ���� ������
	int	iLastFrame;     // ������ ������
	int	iFrameSpeed;    // 1�ʴ� ������ ����(30)
	int	iTickPerFrame;  // 1�������� ƽ ��(160)
	int iNumMesh;		// �޽�������Ʈ ����
	int iMaxWeight;		// ���� �� ����ġ
	int iBindPose;		// ���ε� ���� ���ϸ��̼� ����
};
struct MATRIXINDEX
{	
	OBJECTCLASSTYPE			m_ClassType;	 
	int						m_iIndex;// ���ϸ��̼� ����� �ε��� // �޽���½� ������ ������ ��ġ�� ��� �� ����ġ.
	T_STR			m_szName;	// ���ϸ��̼� ����� �̸�		
	vector<P3NCVERTEX>		m_VertexList;// �޽� ���̽� ����Ʈ
	INode*					m_pINode;
	Object*					m_pObj;	

	void		Add( int iIndex,	T_STR szName )
	{
		m_pObj		= NULL;	
		m_iIndex	= iIndex;
		m_szName	= szName;
	}
	void		SetNode( INode* pNode, MATRIXINDEX* pParentIndex=0 )
	{
		m_pINode		= pNode;
		m_pObj = m_pINode->GetObjectRef();	
		
		m_ClassType = CLASS_GEOM;	
		if ( m_pObj && m_pObj->ClassID()==Class_ID(BONE_CLASS_ID,0))// �� ������Ʈ   
		{
			m_ClassType = CLASS_BONE;
		}
		if( m_pObj && m_pObj->ClassID()==Class_ID(DUMMY_CLASS_ID,0))  // ���� ������Ʈ
		{							
			m_ClassType = CLASS_DUMMY;
		}		
	}
	void					Release(){};
};

class TbsGlobal
{
public:
	Interface*		m_p3dsMax;		// �ƽ� �������̽�	
	INode*			m_pRootNode;
	Interval		m_Interval;		// ������ ����
	TScene			m_Scene;		// �� ���� ����
	TTemplateMap< MATRIXINDEX >		m_MatrixMap;
	vector<Mtl*>	m_pMtlList;		//���͸��� ����Ʈ	
	TCHAR			m_tmpBuffer[MAX_PATH];//�ӽù���
public:
	bool			Initialize( Interface*	p3dsMax=NULL);
	void			PreProcess( INode* pNode );
	void			AddObject( INode* pNode );
	void			AddMtl( INode* pNode );// �ߺ����� �ʰ� ���͸��� �߰�
	int				GetMtlRef(Mtl* pMtl); // pMtl�� ����� �ε����� ��ȯ�Ѵ�.
	void			SetBindPose(bool bBindPose = false ){m_Scene.iBindPose=bBindPose;}
	int				GetIndex( TCHAR* strNodeName );
public:
		void	DumpMatrix3(Matrix3* m, D3D_MATRIX& mat );
		void	DumpPoint3( Point3& pDest, Point3& pSrc );
		bool	TMNegParity( Matrix3 &m );
		bool	EqualPoint2(Point2 p1, Point2 p2);
		bool	EqualPoint3(Point3 p1, Point3 p2);
		bool	EqualPoint4(Point4 p1, Point4 p2);
		TCHAR*	FixupName(MSTR name);
public:
	TbsGlobal(void);
	virtual ~TbsGlobal(void);
};
