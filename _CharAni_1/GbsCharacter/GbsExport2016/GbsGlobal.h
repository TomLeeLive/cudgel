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

// ─────────────────────────────────
//  파이패드 메쉬
// ─────────────────────────────────
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
	int iVersion;		// 파일 버전
	int	iFirstFrame;    // 시작 프레임
	int	iLastFrame;     // 마지막 프레임
	int	iFrameSpeed;    // 1초당 프레임 개수(30)
	int	iTickPerFrame;  // 1프레임의 틱 값(160)
	int iNumMesh;		// 메쉬오브젝트 개수
	int iMaxWeight;		// 정점 당 가중치
	int iBindPose;		// 바인딩 포즈 에니메이션 여부
};
struct MATRIXINDEX
{	
	OBJECTCLASSTYPE			m_ClassType;	 
	int						m_iIndex;// 에니메이션 행렬의 인덱스 // 메쉬출력시 정점의 영향을 미치는 행렬 및 가중치.
	T_STR			m_szName;	// 에니메이션 행렬의 이름		
	vector<P3NCVERTEX>		m_VertexList;// 메쉬 페이스 리스트
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
		if ( m_pObj && m_pObj->ClassID()==Class_ID(BONE_CLASS_ID,0))// 본 오브젝트   
		{
			m_ClassType = CLASS_BONE;
		}
		if( m_pObj && m_pObj->ClassID()==Class_ID(DUMMY_CLASS_ID,0))  // 더미 오브젝트
		{							
			m_ClassType = CLASS_DUMMY;
		}		
	}
	void					Release(){};
};

class TbsGlobal
{
public:
	Interface*		m_p3dsMax;		// 맥스 인터페이스	
	INode*			m_pRootNode;
	Interval		m_Interval;		// 프레임 정보
	TScene			m_Scene;		// 씬 정보 저장
	TTemplateMap< MATRIXINDEX >		m_MatrixMap;
	vector<Mtl*>	m_pMtlList;		//매터리얼 리스트	
	TCHAR			m_tmpBuffer[MAX_PATH];//임시버퍼
public:
	bool			Initialize( Interface*	p3dsMax=NULL);
	void			PreProcess( INode* pNode );
	void			AddObject( INode* pNode );
	void			AddMtl( INode* pNode );// 중복되지 않고 매터리얼 추가
	int				GetMtlRef(Mtl* pMtl); // pMtl이 저장된 인덱스를 반환한다.
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
