#pragma once
#include "gbsExport2016.h"
class GbsWriter
{
public:
	TSTR	m_szFileName;
	Interface* m_p3dsMax;
	FILE*	m_fp;
public:
	void Init(TSTR name,
		Interface* p3dsMax);
	void Release();
	bool Exporter();
	bool ExpScene();
	bool ExpMaterial();
	bool ExpObject();
public:
	GbsWriter();
	virtual ~GbsWriter();
};

