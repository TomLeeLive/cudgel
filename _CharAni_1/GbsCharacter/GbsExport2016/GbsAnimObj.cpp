#include "GbsAnimObj.h"

void TbsAnimObj::GetAnimKeys( INode* pNode, TMesh& pMesh)
{
	BOOL bPosAnim;
	BOOL bRotAnim;
	BOOL bScaleAnim;

	m_iDeltaTick = m_Scene.iTickPerFrame;

	if(CheckForAnimation(pNode, bPosAnim, bRotAnim, bScaleAnim)) 
	{
		if( bPosAnim )
		{
			DumpPosSample( pNode, pMesh);
		}
		if( bRotAnim )
		{
			DumpRotSample( pNode, pMesh );
		}
		if( bScaleAnim )
		{
			DumpScaleSample( pNode, pMesh  );
		}
	}
	
	// Export the visibility track
	Control* visCont = pNode->GetVisController();
	if (visCont) 
	{			
		DumpFloatKeys(visCont, pMesh);		
	}
}
bool TbsAnimObj::GetDecompAffine( TimeValue t, INode* pNode, AffineParts* ap, Point3* rotAxis, float* rotAngle )
{
	Matrix3 tm = pNode->GetNodeTM(t) * Inverse(pNode->GetParentTM(t));
	decomp_affine(tm, ap);

	Point3 vRotAxis;
	float  fRotAngle;
	if( rotAngle != NULL && rotAngle != NULL )
	{
		AngAxisFromQ(ap->q, rotAngle, *rotAxis);
	}
	return true;
}
BOOL TbsAnimObj::CheckForAnimation(INode* pNode, BOOL& bPos, BOOL& bRot, BOOL& bScale)
{
	AffineParts ap;
	Point3 firstPos, firstRotAxis, firstScaleFactor;
	float firstRotAngle;
	GetDecompAffine( m_Interval.Start(), pNode, &ap, &firstRotAxis, &firstRotAngle );
	firstPos			= ap.t;
	firstScaleFactor	= ap.k;	
	
	TimeValue start = m_Interval.Start()+GetTicksPerFrame();
	TimeValue end	= m_Interval.End();

	bPos = bRot = bScale = FALSE;		
	float rotAngle;
	Point3 rotAxis;

	for(TimeValue t=start; t<=end; t+=m_iDeltaTick) 
	{
		GetDecompAffine( t, pNode, &ap, &rotAxis, &rotAngle );		
		
		if (!bPos) 
		{
			if (!EqualPoint3(ap.t, firstPos)) 
			{
				bPos = TRUE;
			}
		}		
		if (!bRot) 
		{
			if (fabs(rotAngle - firstRotAngle) > ALMOST_ZERO) 
			{
				bRot = TRUE;
			}
			else if (!EqualPoint3(rotAxis, firstRotAxis)) 
			{
				bRot = TRUE;
			}
		}
		if (!bScale) 
		{
			if (!EqualPoint3(ap.k, firstScaleFactor)) 
			{
				bScale = TRUE;
			}
		}		
	
		if (bPos && bRot && bScale)		break;
	}
	return bPos || bRot || bScale;
}

void TbsAnimObj::DumpPosSample(INode* pNode, TMesh& pMesh ) 
{	
	TimeValue start = m_Interval.Start();
	TimeValue end	= m_Interval.End();
	AffineParts ap;
	pMesh.m_PosTrack.resize(m_Scene.iLastFrame-m_Scene.iFirstFrame+1);
	int iFrame = 0;
	for( TimeValue t = start; t <= end; t+=m_iDeltaTick, iFrame++ ) 
	{		
		GetDecompAffine( t, pNode, &ap );	
		pMesh.m_PosTrack[iFrame].iTick = t;
		pMesh.m_PosTrack[iFrame].vValue.x = ap.t.x;
		pMesh.m_PosTrack[iFrame].vValue.y = ap.t.z;
		pMesh.m_PosTrack[iFrame].vValue.z = ap.t.y;
	}	
}

void TbsAnimObj::DumpRotSample( INode* pNode, TMesh& pMesh ) 
{		
	TimeValue start = m_Interval.Start();
	TimeValue end	= m_Interval.End();

	AffineParts ap;
	pMesh.m_RotTrack.resize(m_Scene.iLastFrame-m_Scene.iFirstFrame+1);
	int iFrame = 0;
	for( TimeValue t = start; t <= end; t+=m_iDeltaTick, iFrame++ ) 
	{		
		GetDecompAffine( t, pNode, &ap );		
		pMesh.m_RotTrack[iFrame].iTick = t;
		pMesh.m_RotTrack[iFrame].qValue.x = ap.q.x;
		pMesh.m_RotTrack[iFrame].qValue.y = ap.q.z;
		pMesh.m_RotTrack[iFrame].qValue.z = ap.q.y;
		pMesh.m_RotTrack[iFrame].qValue.w = ap.q.w;
	}	
}

void TbsAnimObj::DumpScaleSample( INode* pNode, TMesh& pMesh ) 
{		
	TimeValue start = m_Interval.Start();
	TimeValue end	= m_Interval.End();

	AffineParts ap;
	pMesh.m_SclTrack.resize(m_Scene.iLastFrame-m_Scene.iFirstFrame+1);
	int iFrame = 0;
	for( TimeValue t = start; t <= end; t+=m_iDeltaTick, iFrame++ ) 
	{		
		GetDecompAffine( t, pNode, &ap );
		
		pMesh.m_SclTrack[iFrame].iTick = t;
		pMesh.m_SclTrack[iFrame].vValue.x = ap.k.x;
		pMesh.m_SclTrack[iFrame].vValue.y = ap.k.z;
		pMesh.m_SclTrack[iFrame].vValue.z = ap.k.y;
		pMesh.m_SclTrack[iFrame].qValue.x = ap.u.x;
		pMesh.m_SclTrack[iFrame].qValue.y = ap.u.z;
		pMesh.m_SclTrack[iFrame].qValue.z = ap.u.y;
		pMesh.m_SclTrack[iFrame].qValue.w = ap.u.w;			
	}
}
BOOL TbsAnimObj::IsKnownController(Control* cont)
{
	ulong partA, partB;

	if (!cont)
		return FALSE;

//Listed below are the first ULONG of the 8 byte ID. 
//The second ULONG is 0 for all built-in classes (unless noted otherwise). 
//For example a Class_ID for a TriObject would read:
//Class_ID(TRIOBJ_CLASS_ID, 0);
//Note that only built-in classes should have the second ULONG equal to 0.
//All plug-in developers should use both ULONGs.


	// ClassID는 두개의 ULONG변수로 구성되며 내장된 클래스들은 모두 두번째 
	// 변수는 0의 값을 갖고 있다. 
	// 단, 개발자의 플로그인은 두번째 값을 갖게 된다.
	partA = cont->ClassID().PartA();
	partB = cont->ClassID().PartB();

	if (partB != 0x00)
		return FALSE;

	switch (partA) {
		case TCBINTERP_POSITION_CLASS_ID:
		case TCBINTERP_ROTATION_CLASS_ID:
		case TCBINTERP_SCALE_CLASS_ID:
		case HYBRIDINTERP_POSITION_CLASS_ID:
		case HYBRIDINTERP_ROTATION_CLASS_ID:
		case HYBRIDINTERP_SCALE_CLASS_ID:
		case LININTERP_POSITION_CLASS_ID:
		case LININTERP_ROTATION_CLASS_ID:
		case LININTERP_SCALE_CLASS_ID:
			return TRUE;
	}

	return FALSE;
}
void TbsAnimObj::DumpFloatKeys(Control* cont, TMesh& pMesh) 
{
	if (!cont)		return;	
	IKeyControl *ikc = NULL;	
	ikc = GetKeyControlInterface(cont);	

	// TCB float
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0)) 
	{		
		pMesh.m_VisTrack.resize(ikc->GetNumKeys());
		for(int i=0; i<ikc->GetNumKeys(); i++) 
		{
			ITCBFloatKey key;
			ikc->GetKey(i, &key);	
			pMesh.m_VisTrack[i].iTick = key.time;
			pMesh.m_VisTrack[i].vValue.x = key.val;
		}		
	}
	// Bezier float
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0)) 
	{		
		pMesh.m_VisTrack.resize(ikc->GetNumKeys());
		for( int i=0; i<ikc->GetNumKeys(); i++) 
		{
			IBezFloatKey key;
			ikc->GetKey(i, &key);	
			pMesh.m_VisTrack[i].iTick = key.time;
			pMesh.m_VisTrack[i].vValue.x = key.val;	
		}		
	}
	else if(ikc && cont->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0)) 
	{		
		pMesh.m_VisTrack.resize(ikc->GetNumKeys());
		for(int i=0; i<ikc->GetNumKeys(); i++) 
		{
			ILinFloatKey key;
			ikc->GetKey(i, &key);		
			pMesh.m_VisTrack[i].iTick = key.time;
			pMesh.m_VisTrack[i].vValue.x = key.val;				
		}		
	}
	else 
	{
		TSTR name;
		cont->GetClassName(name);
		
		// If it is animated at all...
		if (cont->IsAnimated()) 
		{
			// Get the range of the controller animation 
			Interval range; 
			// Get range of full animation
			Interval animRange = m_p3dsMax->GetAnimRange(); 
			TimeValue t = cont->GetTimeRange(TIMERANGE_ALL).Start();
			int iFrame = 0;
			while (animRange.InInterval(t)) 
			{
				iFrame++;
				if (range.End() > cont->GetTimeRange(TIMERANGE_ALL).End()) 
				{
					break;
				}
				else
				{
					t = (range.End()/GetTicksPerFrame()) * GetTicksPerFrame();
				}
			}
			pMesh.m_VisTrack.resize(iFrame);


			t = cont->GetTimeRange(TIMERANGE_ALL).Start();
			float value;			
			iFrame = 0;
			// While we are inside the animation... 
			while (animRange.InInterval(t)) 
			{
				// Sample the controller
				range = FOREVER;
				cont->GetValue(t, &value, range);
				
				// Set time to start of controller validity interval 
				t = range.Start();				
				pMesh.m_VisTrack[iFrame].iTick = t;
				pMesh.m_VisTrack[iFrame++].vValue.x = value;
						
				if (range.End() > cont->GetTimeRange(TIMERANGE_ALL).End()) 
				{
					break;
				}
				else 
				{
					//t = (range.End()/GetTicksPerFrame()+GetKeyFrameStep()) * GetTicksPerFrame();
					t = (range.End()/GetTicksPerFrame()) * GetTicksPerFrame();
				}
			}
		}		
	}	
}

void TbsAnimObj::DumpPosKeys(Control* cont, TMesh& pMesh) 
{
	if (!cont)		return;
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	// TCB position
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0))
	{
		int numKeys = ikc->GetNumKeys();
		if (numKeys != 0) 
		{
			pMesh.m_PosTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++)
			{				
				ITCBPoint3Key key;
				ikc->GetKey(i, &key);
				pMesh.m_PosTrack[i].iType	=  TCBINTERP_POSITION_CLASS_ID;
				pMesh.m_PosTrack[i].iTick		= key.time;
				pMesh.m_PosTrack[i].vValue.x	= key.val.x;
				pMesh.m_PosTrack[i].vValue.y	= key.val.z;
				pMesh.m_PosTrack[i].vValue.z	= key.val.y;
				pMesh.m_PosTrack[i].tens		= key.tens;
				pMesh.m_PosTrack[i].cont		= key.cont;
				pMesh.m_PosTrack[i].bias		= key.bias;
				pMesh.m_PosTrack[i].easeIn		= key.easeIn;
				pMesh.m_PosTrack[i].easeOut		= key.easeOut;
			}
		}
	}
//	// Key flags
//#define KEY_SELECTED		IKEY_SELECTED
//#define KEY_XSEL			IKEY_XSEL
//#define KEY_YSEL			IKEY_YSEL
//#define KEY_ZSEL			IKEY_ZSEL
//#define KEY_WSEL			IKEY_WSEL
//#define KEY_ALLSEL			IKEY_ALLSEL
//#define KEY_FLAGGED			IKEY_FLAGGED
//#define KEY_TIME_LOCK		IKEY_TIME_LOCK
	// Bezier position
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
	{
		int numKeys = ikc->GetNumKeys();
		if(numKeys != 0)
		{
			pMesh.m_PosTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++) 
			{
				IBezPoint3Key key;
				ikc->GetKey(i, &key);
				pMesh.m_PosTrack[i].iType	=  HYBRIDINTERP_POSITION_CLASS_ID;
				pMesh.m_PosTrack[i].iTick		= key.time;
				pMesh.m_PosTrack[i].vValue.x	= key.val.x;
				pMesh.m_PosTrack[i].vValue.y	= key.val.z;
				pMesh.m_PosTrack[i].vValue.z	= key.val.y;
				pMesh.m_PosTrack[i].intan.x		= key.intan.x;
				pMesh.m_PosTrack[i].intan.y		= key.intan.z;
				pMesh.m_PosTrack[i].intan.z		= key.intan.y;
				pMesh.m_PosTrack[i].outtan.x		= key.outtan.x;
				pMesh.m_PosTrack[i].outtan.y		= key.outtan.z;
				pMesh.m_PosTrack[i].outtan.z		= key.outtan.y;
				pMesh.m_PosTrack[i].inLength.x	= key.inLength.x;
				pMesh.m_PosTrack[i].inLength.y	= key.inLength.z;
				pMesh.m_PosTrack[i].inLength.z	= key.inLength.y;
				pMesh.m_PosTrack[i].outLength.x	= key.outLength.x;
				pMesh.m_PosTrack[i].outLength.y	= key.outLength.z;
				pMesh.m_PosTrack[i].outLength.z	= key.outLength.y;
				pMesh.m_PosTrack[i].flags			= key.flags;
		
			}			
		}
	}
	// Linear position
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0))
	{
		int numKeys = ikc->GetNumKeys();
		if(numKeys != 0)
		{
			pMesh.m_PosTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++) 
			{
				ILinPoint3Key key;
				ikc->GetKey(i, &key);
				pMesh.m_PosTrack[i].iType	=  LININTERP_POSITION_CLASS_ID;
				pMesh.m_PosTrack[i].iTick		= key.time;
				pMesh.m_PosTrack[i].vValue.x	= key.val.x;
				pMesh.m_PosTrack[i].vValue.y	= key.val.z;
				pMesh.m_PosTrack[i].vValue.z	= key.val.y;
			}			
		}
	}
}

void TbsAnimObj::DumpRotKeys(Control* cont, TMesh& pMesh) 
{
	if (!cont)	return;
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0)) 
	{
		int numKeys = ikc->GetNumKeys();
		if (numKeys != 0) 
		{			
			pMesh.m_RotTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++) 
			{
				ITCBRotKey key;
				ikc->GetKey(i, &key);
				pMesh.m_RotTrack[i].iType	=  TCBINTERP_ROTATION_CLASS_ID;
				pMesh.m_RotTrack[i].iTick		= key.time;
				Quat qR		= QFromAngAxis( key.val.angle, key.val.axis );
				pMesh.m_RotTrack[i].qValue.x	= qR.x;
				pMesh.m_RotTrack[i].qValue.y	= qR.z;
				pMesh.m_RotTrack[i].qValue.z	= qR.y;
				pMesh.m_RotTrack[i].qValue.w	= qR.w;
				pMesh.m_RotTrack[i].tens		= key.tens;
				pMesh.m_RotTrack[i].cont		= key.cont;
				pMesh.m_RotTrack[i].bias		= key.bias;
				pMesh.m_RotTrack[i].easeIn		= key.easeIn;
				pMesh.m_RotTrack[i].easeOut		= key.easeOut;				
			}			
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
	{
		int numKeys = ikc->GetNumKeys();
		if (numKeys != 0) 
		{
			pMesh.m_RotTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++) 
			{
				IBezQuatKey key;
				ikc->GetKey(i, &key);
				pMesh.m_RotTrack[i].iType	= HYBRIDINTERP_ROTATION_CLASS_ID;
				pMesh.m_RotTrack[i].iTick		= key.time;
				pMesh.m_RotTrack[i].qValue.x	= key.val.x;
				pMesh.m_RotTrack[i].qValue.y	= key.val.z;
				pMesh.m_RotTrack[i].qValue.z	= key.val.y;
				pMesh.m_RotTrack[i].qValue.w	= key.val.w;
			}		
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0))
	{
		int numKeys = ikc->GetNumKeys();
		if (numKeys != 0) 
		{
			pMesh.m_RotTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++) 
			{
				ILinRotKey key;
				ikc->GetKey(i, &key);
				pMesh.m_RotTrack[i].iType	=  LININTERP_ROTATION_CLASS_ID;
				pMesh.m_RotTrack[i].iTick		= key.time;
				pMesh.m_RotTrack[i].qValue.x	= key.val.x;
				pMesh.m_RotTrack[i].qValue.y	= key.val.z;
				pMesh.m_RotTrack[i].qValue.z	= key.val.y;
				pMesh.m_RotTrack[i].qValue.w	= key.val.w;
			}		
		}
	}
}

void TbsAnimObj::DumpScaleKeys(Control* cont, TMesh& pMesh) 
{
	if (!cont)		return;	
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_SCALE_CLASS_ID, 0))
	{
		int numKeys = ikc->GetNumKeys();
		if (numKeys != 0) 
		{
			pMesh.m_SclTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++) 
			{
				ITCBScaleKey key;
				ikc->GetKey(i, &key);
				pMesh.m_SclTrack[i].iType		=  TCBINTERP_SCALE_CLASS_ID;
				pMesh.m_SclTrack[i].iTick		= key.time;
				
				pMesh.m_SclTrack[i].vValue.x	= key.val.s.x;
				pMesh.m_SclTrack[i].vValue.y	= key.val.s.z;
				pMesh.m_SclTrack[i].vValue.z	= key.val.s.y;				

				pMesh.m_SclTrack[i].qValue.x	= key.val.q.x;
				pMesh.m_SclTrack[i].qValue.y	= key.val.q.z;
				pMesh.m_SclTrack[i].qValue.z	= key.val.q.y;
				pMesh.m_SclTrack[i].qValue.w	= key.val.q.w;

				pMesh.m_SclTrack[i].tens		= key.tens;
				pMesh.m_SclTrack[i].cont		= key.cont;
				pMesh.m_SclTrack[i].bias		= key.bias;
				pMesh.m_SclTrack[i].easeIn		= key.easeIn;
				pMesh.m_SclTrack[i].easeOut		= key.easeOut;
			}		
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0)) 
	{
		int numKeys = ikc->GetNumKeys();
		if (numKeys != 0) 
		{
			pMesh.m_SclTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++)
			{
				IBezScaleKey key;
				ikc->GetKey(i, &key);
				pMesh.m_SclTrack[i].iType	=  HYBRIDINTERP_SCALE_CLASS_ID;
				pMesh.m_SclTrack[i].iTick		= key.time;
				
				pMesh.m_SclTrack[i].vValue.x	= key.val.s.x;
				pMesh.m_SclTrack[i].vValue.y	= key.val.s.z;
				pMesh.m_SclTrack[i].vValue.z	= key.val.s.y;				

				pMesh.m_SclTrack[i].qValue.x	= key.val.q.x;
				pMesh.m_SclTrack[i].qValue.y	= key.val.q.z;
				pMesh.m_SclTrack[i].qValue.z	= key.val.q.y;
				pMesh.m_SclTrack[i].qValue.w	= key.val.q.w;

				pMesh.m_SclTrack[i].intan.x		= key.intan.x;
				pMesh.m_SclTrack[i].intan.y		= key.intan.z;
				pMesh.m_SclTrack[i].intan.z		= key.intan.y;
				pMesh.m_SclTrack[i].outtan.x		= key.outtan.x;
				pMesh.m_SclTrack[i].outtan.y		= key.outtan.z;
				pMesh.m_SclTrack[i].outtan.z		= key.outtan.y;
				pMesh.m_SclTrack[i].flags			= key.flags;
			}			
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0)) 
	{
		int numKeys = ikc->GetNumKeys();
		if (numKeys != 0) 
		{
			pMesh.m_SclTrack.resize(numKeys);
			for(int i=0; i<numKeys; i++) 
			{
				ILinScaleKey key;
				ikc->GetKey(i, &key);				
				pMesh.m_SclTrack[i].iType		= LININTERP_SCALE_CLASS_ID;
				pMesh.m_SclTrack[i].iTick		= key.time;
				
				pMesh.m_SclTrack[i].vValue.x	= key.val.s.x;
				pMesh.m_SclTrack[i].vValue.y	= key.val.s.z;
				pMesh.m_SclTrack[i].vValue.z	= key.val.s.y;				

				pMesh.m_SclTrack[i].qValue.x	= key.val.q.x;
				pMesh.m_SclTrack[i].qValue.y	= key.val.q.z;
				pMesh.m_SclTrack[i].qValue.z	= key.val.q.y;
				pMesh.m_SclTrack[i].qValue.w	= key.val.q.w;
			}			
		}
	}
}
TbsAnimObj::TbsAnimObj(void)
{
	m_iDeltaTick = 160;
}

TbsAnimObj::~TbsAnimObj(void)
{
}
