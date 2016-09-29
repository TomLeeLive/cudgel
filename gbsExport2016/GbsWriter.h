#pragma once
#include "gbsExport2016.h"

#include <vector>
using namespace std;

struct GScene
{
	int iVersion;
	int iFirstFrame;
	int iLastFrame;
	int iFrameSpeed;
	int iTickPerFrame;
	int iNumMtl;
	int iNumObject;
};

struct GTex
{
	int iMapType;
	TSTR szTexName;
};
struct GMtl
{
	Mtl* pMtl;
	int iNumSubMtls;
	int iMapType;
	vector<GTex> texlist;
	vector<GMtl> subMtls;
};


class GbsWriter
{
public:
	TSTR			m_szFileName;
	Interface*		m_p3dsMax;
	INode*			m_pRootNode;
	vector <INode*> m_ObjectList;
	vector <Mtl*>	m_MtlList;
	vector <GMtl>	m_gMtlList;
	Interval		m_Interval;
	GScene			m_Scene;
	FILE*			m_fp;
public:
	void Init(TSTR name,
		Interface* p3dsMax);

	void PreProcess(INode* m_pRootNode);
	void AddObject(INode* pNode);
	void AddMtl(INode* pNode);



	void Release();
	bool Exporter();
	bool ExpScene();
	bool ExpMaterial();
	bool ExpObject();

	void GetTexture(Mtl* pMtl, GMtl& gSubMtl);

public:
	GbsWriter();
	virtual ~GbsWriter();
};

