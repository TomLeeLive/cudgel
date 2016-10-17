#pragma once
#include "GObjStd.h"
#include "GMesh.h"

class GAnimation
{
public:
	D3DXMATRIX	Interpolate(GMesh* pMesh, D3DXMATRIX* matParents, float fFrameTick, TScene tScene);
	D3DXMATRIX	Update(float fElapseTime, GMesh* pMesh, D3DXMATRIX &matWorld, TScene &tScene);
	bool		GetAnimationTrack(float fFrame, vector<shared_ptr<TAnimTrack>>& pTrackList, TAnimTrack** pStartTrack, TAnimTrack** pEndTrack);
public:
	GAnimation(void);
	virtual ~GAnimation(void);
};

