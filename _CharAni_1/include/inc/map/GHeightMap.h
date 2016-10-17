#pragma once
#include "GMap.h"

class GHeightMap : public GMap
{
public:
	vector<float>		m_fHeightList;
public:
	bool		CreateHeightMap(TCHAR* strHeightMap);
	float		GetHeightOfVertex(UINT Index);
	D3DXVECTOR4 GetColorOfVertex(UINT Index);	
	float		GetHeightmap(int row, int col);
public:
	GHeightMap(void);
	virtual ~GHeightMap(void);
};
