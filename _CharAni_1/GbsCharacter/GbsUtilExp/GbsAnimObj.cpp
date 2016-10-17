#include "GbsAnimObj.h"

void TbsAnimObj::GetAnimKeys( INode* pNode, TMesh* pMesh)
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

void TbsAnimObj::DumpPosSample(INode* pNode, TMesh* pMesh ) 
{	
	TimeValue start = m_Interval.Start();
	TimeValue end	= m_Interval.End();
	AffineParts ap;
	
	for( TimeValue t = start; t <= end; t+=m_iDeltaTick ) 
	{		
		GetDecompAffine( t, pNode, &ap );

		TAnimTrack pAnim;		
		pAnim.iTick = t;
		pAnim.vValue.x = ap.t.x;
		pAnim.vValue.y = ap.t.z;
		pAnim.vValue.z = ap.t.y;
		pMesh->m_PosTrack.push_back(pAnim);	
	}	
}

void TbsAnimObj::DumpRotSample( INode* pNode, TMesh* pMesh ) 
{		
	TimeValue start = m_Interval.Start();
	TimeValue end	= m_Interval.End();

	AffineParts ap;
	for( TimeValue t = start; t <= end; t+=m_iDeltaTick ) 
	{		
		GetDecompAffine( t, pNode, &ap );
	
		TAnimTrack pAnim;
		pAnim.iTick = t;
		pAnim.qValue.x = ap.q.x;
		pAnim.qValue.y = ap.q.z;
		pAnim.qValue.z = ap.q.y;
		pAnim.qValue.w = ap.q.w;		
		pMesh->m_RotTrack.push_back(pAnim);
	}
}

void TbsAnimObj::DumpScaleSample( INode* pNode, TMesh* pMesh ) 
{		
	TimeValue start = m_Interval.Start();
	TimeValue end	= m_Interval.End();

	AffineParts ap;
	for( TimeValue t=start; t<=end; t+=m_iDeltaTick ) 
	{		
		GetDecompAffine( t, pNode, &ap );

		TAnimTrack Anim;	
		Anim.iTick = t;
		Anim.vValue.x = ap.k.x;
		Anim.vValue.y = ap.k.z;
		Anim.vValue.z = ap.k.y;
		Anim.qValue.x = ap.u.x;
		Anim.qValue.y = ap.u.z;
		Anim.qValue.z = ap.u.y;
		Anim.qValue.w = ap.u.w;	
		pMesh->m_SclTrack.push_back(Anim);
	}
}
//BOOL TbsAnimObj::IsKnownController(Control* cont)
//{
//	ulong partA, partB;
//
//	if (!cont)
//		return FALSE;
//
////Listed below are the first ULONG of the 8 byte ID. 
////The second ULONG is 0 for all built-in classes (unless noted otherwise). 
////For example a Class_ID for a TriObject would read:
////Class_ID(TRIOBJ_CLASS_ID, 0);
////Note that only built-in classes should have the second ULONG equal to 0.
////All plug-in developers should use both ULONGs.
//
//
//	// ClassID는 두개의 ULONG변수로 구성되며 내장된 클래스들은 모두 두번째 
//	// 변수는 0의 값을 갖고 있다. 
//	// 단, 개발자의 플로그인은 두번째 값을 갖게 된다.
//	partA = cont->ClassID().PartA();
//	partB = cont->ClassID().PartB();
//
//	if (partB != 0x00)
//		return FALSE;
//
//	switch (partA) {
//		case TCBINTERP_POSITION_CLASS_ID:
//		case TCBINTERP_ROTATION_CLASS_ID:
//		case TCBINTERP_SCALE_CLASS_ID:
//		case HYBRIDINTERP_POSITION_CLASS_ID:
//		case HYBRIDINTERP_ROTATION_CLASS_ID:
//		case HYBRIDINTERP_SCALE_CLASS_ID:
//		case LININTERP_POSITION_CLASS_ID:
//		case LININTERP_ROTATION_CLASS_ID:
//		case LININTERP_SCALE_CLASS_ID:
//			return TRUE;
//	}
//
//	return FALSE;
//}
void TbsAnimObj::DumpFloatKeys(Control* cont, TMesh* pMesh) 
{
	if (!cont)		return;	
	int i;	
	IKeyControl *ikc = NULL;	
	ikc = GetKeyControlInterface(cont);
	
	TAnimTrack Anim;

	// TCB float
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0)) 
	{		
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			ITCBFloatKey key;
			ikc->GetKey(i, &key);	
			Anim.iTick = key.time;
			Anim.vValue.x = key.val;
			pMesh->m_VisTrack.push_back( Anim );
		}		
	}
	// Bezier float
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0)) {		
		for (i=0; i<ikc->GetNumKeys(); i++) {
			IBezFloatKey key;
			ikc->GetKey(i, &key);	
			Anim.iTick = key.time;
			Anim.vValue.x = key.val;
			pMesh->m_VisTrack.push_back( Anim );
			
		}		
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0)) {		
		for (i=0; i<ikc->GetNumKeys(); i++) 
		{
			ILinFloatKey key;
			ikc->GetKey(i, &key);		
			Anim.iTick = key.time;
			Anim.vValue.x = key.val;
			pMesh->m_VisTrack.push_back( Anim );
		}		
	}
	else 
	{
		TSTR name;
		cont->GetClassName(name);
		
		// If it is animated at all...
		if (cont->IsAnimated()) {
			// Get the range of the controller animation 
			Interval range; 
			// Get range of full animation
			Interval animRange = m_p3dsMax->GetAnimRange(); 
			TimeValue t = cont->GetTimeRange(TIMERANGE_ALL).Start();
			float value;
			
			// While we are inside the animation... 
			while (animRange.InInterval(t)) 
			{
				// Sample the controller
				range = FOREVER;
				cont->GetValue(t, &value, range);
				
				// Set time to start of controller validity interval 
				t = range.Start();
				
				Anim.iTick = t;
				Anim.vValue.x = value;
				pMesh->m_VisTrack.push_back( Anim );
						
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
//
//void TbsAnimObj::DumpPosKeys(Control* cont, TMesh* pMesh) 
//{
//	if (!cont)
//		return;
//	
//	int i;
//	IKeyControl *ikc = GetKeyControlInterface(cont);
//	
//	// TCB position
//	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0))
//	{
//		int numKeys = ikc->GetNumKeys();
//		if (numKeys != 0) 
//		{
//			for (i=0; i<numKeys; i++)
//			{				
//				ITCBPoint3Key key;
//				ikc->GetKey(i, &key);
//
//				TAnimTrack Anim;
//				Anim.m_dwType		=  T_TCBINTERP_POSITION_CLASS_ID;
//				Anim.iTick		= key.time;
//				Anim.vValue.x	= key.val.x;
//				Anim.vValue.y	= key.val.z;
//				Anim.vValue.z	= key.val.y;
//				Anim.tens			= key.tens;
//				Anim.cont			= key.cont;
//				Anim.bias			= key.bias;
//				Anim.easeIn			= key.easeIn;
//				Anim.easeOut		= key.easeOut;
//				pMesh->m_PosTrack.push_back(Anim);	
//			}
//		}
//	}
////	// Key flags
////#define KEY_SELECTED		IKEY_SELECTED
////#define KEY_XSEL			IKEY_XSEL
////#define KEY_YSEL			IKEY_YSEL
////#define KEY_ZSEL			IKEY_ZSEL
////#define KEY_WSEL			IKEY_WSEL
////#define KEY_ALLSEL			IKEY_ALLSEL
////#define KEY_FLAGGED			IKEY_FLAGGED
////#define KEY_TIME_LOCK		IKEY_TIME_LOCK
//	// Bezier position
//	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
//	{
//		int numKeys = ikc->GetNumKeys();
//		if(numKeys != 0)
//		{
//			for (i=0; i<numKeys; i++) 
//			{
//				IBezPoint3Key key;
//				ikc->GetKey(i, &key);
//				TAnimTrack Anim;
//				Anim.m_dwType		=  T_HYBRIDINTERP_POSITION_CLASS_ID;
//				Anim.iTick		= key.time;
//				Anim.vValue.x	= key.val.x;
//				Anim.vValue.y	= key.val.z;
//				Anim.vValue.z	= key.val.y;
//				Anim.m_vIntan.x		= key.intan.x;
//				Anim.m_vIntan.y		= key.intan.z;
//				Anim.m_vIntan.z		= key.intan.y;
//				Anim.m_vOuttan.x		= key.outtan.x;
//				Anim.m_vOuttan.y		= key.outtan.z;
//				Anim.m_vOuttan.z		= key.outtan.y;
//				Anim.m_inLength.x		= key.inLength.x;
//				Anim.m_inLength.y		= key.inLength.z;
//				Anim.m_inLength.z		= key.inLength.y;
//				Anim.m_outLength.x		= key.outLength.x;
//				Anim.m_outLength.y		= key.outLength.z;
//				Anim.m_outLength.z		= key.outLength.y;
//				Anim.flags			= key.flags;
//				pMesh->m_PosTrack.push_back(Anim);				
//			}			
//		}
//	}
//	// Linear position
//	else if (ikc && cont->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0))
//	{
//		int numKeys = ikc->GetNumKeys();
//		if(numKeys != 0)
//		{
//			for (i=0; i<numKeys; i++) 
//			{
//				ILinPoint3Key key;
//				ikc->GetKey(i, &key);
//				TAnimTrack Anim;
//				Anim.m_dwType		=  T_LININTERP_POSITION_CLASS_ID;
//				Anim.iTick		= key.time;
//				Anim.vValue.x	= key.val.x;
//				Anim.vValue.y	= key.val.z;
//				Anim.vValue.z	= key.val.y;
//				pMesh->m_PosTrack.push_back(Anim);	
//			}			
//		}
//	}
//}
//
//void TbsAnimObj::DumpRotKeys(Control* cont, TMesh* pMesh) 
//{
//	if (!cont)	return;
//	
//	int i;
//	IKeyControl *ikc = GetKeyControlInterface(cont);
//	
//	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0)) 
//	{
//		int numKeys = ikc->GetNumKeys();
//		if (numKeys != 0) 
//		{			
//			for (i=0; i<numKeys; i++) 
//			{
//				ITCBRotKey key;
//				ikc->GetKey(i, &key);
//				TAnimTrack Anim;
//				Anim.m_dwType		=  T_TCBINTERP_ROTATION_CLASS_ID;
//				Anim.iTick		= key.time;
//				Quat qR				= QFromAngAxis( key.val.angle, key.val.axis );
//				Anim.qValue.x	= qR.x;
//				Anim.qValue.y	= qR.z;
//				Anim.qValue.z	= qR.y;
//				Anim.qValue.w	= qR.w;
//				Anim.tens			= key.tens;
//				Anim.cont			= key.cont;
//				Anim.bias			= key.bias;
//				Anim.easeIn			= key.easeIn;
//				Anim.easeOut		= key.easeOut;
//				pMesh->m_PosTrack.push_back(Anim);
//			}			
//		}
//	}
//	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
//	{
//		int numKeys = ikc->GetNumKeys();
//		if (numKeys != 0) 
//		{
//			for (i=0; i<numKeys; i++) 
//			{
//				IBezQuatKey key;
//				ikc->GetKey(i, &key);
//				TAnimTrack Anim;
//				Anim.m_dwType		= T_HYBRIDINTERP_ROTATION_CLASS_ID;
//				Anim.iTick		= key.time;
//				Anim.qValue.x	= key.val.x;
//				Anim.qValue.y	= key.val.z;
//				Anim.qValue.z	= key.val.y;
//				Anim.qValue.w	= key.val.w;
//				pMesh->m_PosTrack.push_back(Anim);
//			}		
//		}
//	}
//	else if (ikc && cont->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0))
//	{
//		int numKeys = ikc->GetNumKeys();
//		if (numKeys != 0) 
//		{
//			for (i=0; i<numKeys; i++) 
//			{
//				ILinRotKey key;
//				ikc->GetKey(i, &key);
//				TAnimTrack Anim;
//				Anim.m_dwType		=  T_LININTERP_ROTATION_CLASS_ID;
//				Anim.iTick		= key.time;
//				Anim.qValue.x	= key.val.x;
//				Anim.qValue.y	= key.val.z;
//				Anim.qValue.z	= key.val.y;
//				Anim.qValue.w	= key.val.w;
//				pMesh->m_PosTrack.push_back(Anim);
//			}		
//		}
//	}
//}
//
//void TbsAnimObj::DumpScaleKeys(Control* cont, TMesh* pMesh) 
//{
//	if (!cont)
//		return;
//	
//	int i;
//	IKeyControl *ikc = GetKeyControlInterface(cont);
//	
//	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_SCALE_CLASS_ID, 0))
//	{
//		int numKeys = ikc->GetNumKeys();
//		if (numKeys != 0) 
//		{
//			for (i=0; i<numKeys; i++) 
//			{
//				ITCBScaleKey key;
//				ikc->GetKey(i, &key);
//				TAnimTrack Anim;
//				Anim.m_dwType		=  T_TCBINTERP_SCALE_CLASS_ID;
//				Anim.iTick		= key.time;
//				
//				Anim.vValue.x	= key.val.s.x;
//				Anim.vValue.y	= key.val.s.z;
//				Anim.vValue.z	= key.val.s.y;				
//
//				Anim.qValue.x	= key.val.q.x;
//				Anim.qValue.y	= key.val.q.z;
//				Anim.qValue.z	= key.val.q.y;
//				Anim.qValue.w	= key.val.q.w;
//
//				Anim.tens			= key.tens;
//				Anim.cont			= key.cont;
//				Anim.bias			= key.bias;
//				Anim.easeIn			= key.easeIn;
//				Anim.easeOut		= key.easeOut;
//				pMesh->m_PosTrack.push_back(Anim);
//			}		
//		}
//	}
//	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0)) 
//	{
//		int numKeys = ikc->GetNumKeys();
//		if (numKeys != 0) 
//		{
//			for (i=0; i<numKeys; i++)
//			{
//				IBezScaleKey key;
//				ikc->GetKey(i, &key);
//
//				TAnimTrack Anim;
//				Anim.m_dwType		=  T_HYBRIDINTERP_SCALE_CLASS_ID;
//				Anim.iTick		= key.time;
//				
//				Anim.vValue.x	= key.val.s.x;
//				Anim.vValue.y	= key.val.s.z;
//				Anim.vValue.z	= key.val.s.y;				
//
//				Anim.qValue.x	= key.val.q.x;
//				Anim.qValue.y	= key.val.q.z;
//				Anim.qValue.z	= key.val.q.y;
//				Anim.qValue.w	= key.val.q.w;
//
//				Anim.m_vIntan.x		= key.intan.x;
//				Anim.m_vIntan.y		= key.intan.z;
//				Anim.m_vIntan.z		= key.intan.y;
//				Anim.m_vOuttan.x		= key.outtan.x;
//				Anim.m_vOuttan.y		= key.outtan.z;
//				Anim.m_vOuttan.z		= key.outtan.y;
//				Anim.flags			= key.flags;
//				pMesh->m_PosTrack.push_back(Anim);
//			}			
//		}
//	}
//	else if (ikc && cont->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0)) 
//	{
//		int numKeys = ikc->GetNumKeys();
//		if (numKeys != 0) 
//		{
//			for (i=0; i<numKeys; i++) 
//			{
//				ILinScaleKey key;
//				ikc->GetKey(i, &key);
//				TAnimTrack Anim;
//				Anim.m_dwType		= T_LININTERP_SCALE_CLASS_ID;
//				Anim.iTick		= key.time;
//				
//				Anim.vValue.x	= key.val.s.x;
//				Anim.vValue.y	= key.val.s.z;
//				Anim.vValue.z	= key.val.s.y;				
//
//				Anim.qValue.x	= key.val.q.x;
//				Anim.qValue.y	= key.val.q.z;
//				Anim.qValue.z	= key.val.q.y;
//				Anim.qValue.w	= key.val.q.w;
//				pMesh->m_PosTrack.push_back(Anim);
//			}			
//		}
//	}
//}
TbsAnimObj::TbsAnimObj(void)
{
	m_iDeltaTick = 160;
}

TbsAnimObj::~TbsAnimObj(void)
{
}
