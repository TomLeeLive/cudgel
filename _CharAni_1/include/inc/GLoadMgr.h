#pragma once
#include <windows.h>

class GCallBackBase
{
	public:
		virtual bool Execute() const =0;
};

template <class GObjectType>
class GLoadCallBack : public GCallBackBase 
{
	public:
		GLoadCallBack() 
		{			
			m_pFunction = 0;
			m_hThread = 0;
		}

		typedef bool (GObjectType::*FUNCTIONCALL)();		
		
		virtual bool Execute() const 
		{
			if (m_pFunction) return (m_pObjectType->*m_pFunction)();
			return false;
		}		
		void SetCallback (GObjectType	*pObjectType, 
						  FUNCTIONCALL   pFunction )
		{
			m_pObjectType		= pObjectType;
			m_pFunction			= pFunction;
		}
	public:
		static DWORD	WINAPI LoadMgrThread(void* lParam);
		
		DWORD	m_dwThreadID;
		HANDLE	m_hThread;
		bool	m_bEndOfLoadData;	

	public:	
		bool	NewThread();
		bool	SetThread();
		bool	EndThread();
	private:
		GObjectType  *m_pObjectType;
		FUNCTIONCALL  m_pFunction;
};

template <class GObjectType >
GLoadCallBack<GObjectType> SetProc(GObjectType pObjType)
{
	return GLoadCallBack<GObjectType>(&pObjType);
}
template <class GObjectType>
DWORD  WINAPI GLoadCallBack<GObjectType>::LoadMgrThread(void* lParam)
{	
	 if(lParam)
	 {  		 
		 GLoadCallBack* pLoadMgr=(GLoadCallBack*)lParam;			
		 pLoadMgr->Execute();	 
	 }
	 return 0;
}

template <class GObjectType>
bool GLoadCallBack<GObjectType>::NewThread()
{
	m_bEndOfLoadData = false;
	m_hThread = CreateThread( NULL, 0, GLoadCallBack::LoadMgrThread, this, CREATE_SUSPENDED, &m_dwThreadID );
	if( m_hThread == NULL )
	{
		return false;
	}	
	return true;
}
template <class GObjectType>
bool GLoadCallBack<GObjectType>::SetThread()
{
	ResumeThread(m_hThread);	
	return true;
}
template <class GObjectType>
bool GLoadCallBack<GObjectType>::EndThread()
{
	TerminateThread(m_hThread,m_dwThreadID);
	CloseHandle( m_hThread );
	return true;
}
