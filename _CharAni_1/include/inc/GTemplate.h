#pragma once
#include "GBASISStd.h"
#include "GShape.h"
namespace GBASIS {

	template< typename T > void Swap(T* a, T* b)
	{
		T Temp;
		Temp = *a;
		*a = *b;
		*b = Temp;
	}
	template<typename GContainer>
	inline void stl_wipe(GContainer& container)
	{
		for (GContainer::iterator i = container.begin(); i != container.end(); ++i)
			delete *i;
		container.clear();
	}

	template<typename T>
	void stl_wipe_vector(vector<T>& rVector)
	{
		vector<T> emptyData;
		rVector.swap(emptyData);
	}

	////////////////////////////////////////////////////////
	// 해당 객체를 sdl::map으로 관리한다
	////////////////////////////////////////////////////////
	template< class Child >	class GTemplateMap
	{
	public:
		typedef unordered_map <int, Child*>			TemplateMap;
		typedef typename TemplateMap::iterator		TemplateMapItor;
		TemplateMapItor								TItor;
		TemplateMap									GMap;
		int											m_iCurIndex;
	public:
		// 초기화 한다.
		virtual bool		Init();
		// 인덱스를 통하여 객체를 리턴받는다.
		virtual Child*		GetPtr(int index);
		// 객체이름으로 객체를 리턴받는다.
		virtual Child*		GetPtr(const TCHAR* szName);
		// 객체 포인터로 저장된 인덱스를 리턴받는다.
		virtual int			GetID(Child*);
		// 객체이름으로 저장된 인덱스를 리턴받는다.
		virtual int			GetID(const TCHAR* szName);
		// 전체 저장된 갯수를 얻는다.
		virtual int			Count();
		// 전체 저장된 객체를 삭제한다.
		virtual bool		Release();
		// 인덱스에 해당하는 객체를 삭제한다.
		virtual bool		Delete(int iIndex);
		// pPoint에 해당하는 객체를 삭제한다.
		virtual bool		Delete(Child *pPoint);
		virtual bool		Frame();
		virtual bool		Render(ID3D11DeviceContext*    pContext);

	public:
		GTemplateMap();
		virtual ~GTemplateMap();
	};

	template < class Child >
	int GTemplateMap< Child >::GetID(Child* pChild)
	{
		int iIndex = -1;
		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			Child *pPoint = (Child *)(*itor).second;
			if (pChild == pPoint)
			{
				iIndex = (*itor).first;
				break;
			}
		}
		return iIndex;
	}
	template < class Child >
	int GTemplateMap< Child >::GetID(const TCHAR* szName)
	{
		int iIndex = -1;
		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			Child *pPoint = (Child *)(*itor).second;
			if (!_tcsicmp(pPoint->m_szName.c_str(), szName))
			{
				iIndex = (*itor).first;
				break;
			}
		}
		return iIndex;
	}

	template < class Child >
	bool GTemplateMap< Child >::Init()
	{
		m_iCurIndex = 0;
		GMap.clear();
		return true;
	}
	template < class Child >
	int GTemplateMap< Child >::Count()
	{
		return (int)GMap.size();
	}

	template < class Child >
	bool GTemplateMap< Child >::Frame()
	{
		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			Child *pPoint = (Child *)(*itor).second;
			if (pPoint)
				pPoint->Frame();
		}
		return true;
	}
	template < class Child >
	bool GTemplateMap< Child >::Render(ID3D11DeviceContext*    pContext)
	{
		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			Child *pPoint = (Child *)(*itor).second;
			if (pPoint)
				pPoint->Render(pContext);
		}
		return true;
	}
	template < class Child >
	bool GTemplateMap< Child >::Release()
	{
		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			Child *pPoint = (Child *)(*itor).second;
			if (pPoint)
				pPoint->Release();
			else
				return false;
			delete pPoint;
		}
		GMap.clear();
		m_iCurIndex = 0;
		return true;
	}
	template < class Child >
	Child *GTemplateMap< Child >::GetPtr(int iIndex)
	{
		TemplateMapItor itor = GMap.find(iIndex);
		if (itor == GMap.end()) return NULL;
		Child *pPoint = (*itor).second;
		return pPoint;
	}
	template < class Child >
	Child* GTemplateMap< Child >::GetPtr(const TCHAR* szName)
	{
		for (TemplateMapItor itor = GMap.begin(); itor != GMap.end(); itor++)
		{
			Child *pPoint = (Child *)(*itor).second;
			if (!_tcsicmp(pPoint->m_szName.c_str(), szName))
			{
				return pPoint;
			}
		}
		return NULL;
	}
	template < class Child >
	bool GTemplateMap< Child >::Delete(int iIndex)
	{
		Child *pPoint = GetPtr(iIndex);
		if (pPoint)
		{
			pPoint->Release();
			GMap.erase(iIndex);
		}
		return true;
	}
	template < class Child >
	bool GTemplateMap< Child >::Delete(Child *pPoint)
	{
		if (pPoint)
		{
			pPoint->Release();
			GMap.erase(GetID(pPoint));
		}
		return true;
	}
	template < class Child >
	GTemplateMap< Child >::GTemplateMap()
	{
		m_iCurIndex = 0;
		GMap.clear();
	}

	template < class Child >
	GTemplateMap< Child >::~GTemplateMap()
	{
		Release();
	}
}