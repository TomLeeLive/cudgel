//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "GbsUtilExp.h"
#include "GbsWriter.h"
#include "GSkinExp.h"
#define tbsUtilExp_CLASS_ID	Class_ID(0x3a6f0563, 0x86f24c74)


class tbsUtilExp : public UtilityObj 
{
public:
	TbsWriter		m_tbsWriter;
	HWND			hPanel;
	IUtil			*iu;
	Interface		*ip;
	//Constructor/Destructor
	tbsUtilExp();
	virtual ~tbsUtilExp();

	virtual void DeleteThis() { }
	
	virtual void BeginEditParams(Interface *ip,IUtil *iu);
	virtual void EndEditParams(Interface *ip,IUtil *iu);

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);
	
	void SelectionSetChanged(Interface *ip, IUtil *iu);

	// Singleton access
	static tbsUtilExp* GetInstance() { 
		static tbsUtilExp thetbsUtilExp;
		return &thetbsUtilExp; 
	}

private:

	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};


class tbsUtilExpClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return tbsUtilExp::GetInstance(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return tbsUtilExp_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("gbsUtilExp"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GettbsUtilExpDesc() { 
	static tbsUtilExpClassDesc tbsUtilExpDesc;
	return &tbsUtilExpDesc; 
}




//--- tbsUtilExp -------------------------------------------------------
tbsUtilExp::tbsUtilExp()
	: hPanel(nullptr)
	, iu(nullptr)
{
	
}

tbsUtilExp::~tbsUtilExp()
{

}

void tbsUtilExp::BeginEditParams(Interface* ip,IUtil* iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		DlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void tbsUtilExp::EndEditParams(Interface* ip,IUtil*)
{
	this->iu = NULL;
	this->ip = NULL;
	this->iu = nullptr;
	ip->DeleteRollupPage(hPanel);
	hPanel = nullptr;
}

void tbsUtilExp::Init(HWND hWnd)
{
	if (I_SkinExp.Init() == false) return;
	if (I_Writer.Initialize(ip) ==false) return;

	if (I_Writer.m_MatrixMap.Count() > 0)
	{
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_SKINEXP), true);
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_MATRIXEXP), true);
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_OBJEXPORT), true);
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_UPDATE), false);
	}
	else
	{
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_SKINEXP), false);
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_MATRIXEXP), false);
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_OBJEXPORT), false);
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_UPDATE), true);
	}
	//SendMessage(tbsUtilExp::GetInstance()->hPanel, BM_SETCHECK, BST_CHECKED, 0);
	//CheckDlgButton(theTbsUtilExporter.hPanel, IDC_BINDPOSE, TRUE );
	//EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_BINDPOSE), false);

}

void tbsUtilExp::Destroy(HWND hWnd)
{
	I_SkinExp.Release();
	I_Writer.Release();
}

void tbsUtilExp::SelectionSetChanged(Interface *ip, IUtil *iu)
{	
	if (ip->GetSelNodeCount() <= 0)
	{
		return;
	}
	if (I_Writer.Initialize(ip) == false)
	{
		return;
	}
	I_SkinExp.Release();

	for (int iSelectObj = 0; iSelectObj < ip->GetSelNodeCount(); iSelectObj++)
	{
		I_SkinExp.GetNodeInfo(ip->GetSelNode(iSelectObj), ip->GetTime());
	}

	if (I_Writer.m_MatrixMap.Count() > 0)
	{
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_MATRIXEXP), true);
		if (ip->GetSelNodeCount() > 0)
		{
			EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_SKINEXP), true);
		}
		else
		{
			EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_SKINEXP), false);
		}
	}
	else
	{
		EnableWindow(GetDlgItem(tbsUtilExp::GetInstance()->hPanel, IDC_MATRIXEXP), false);
	}
}

INT_PTR CALLBACK tbsUtilExp::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
			tbsUtilExp::GetInstance()->Init(hWnd);
			break;
		case WM_DESTROY:
			tbsUtilExp::GetInstance()->Destroy(hWnd);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OBJEXPORT:
				{
					I_Writer.Initialize(tbsUtilExp::GetInstance()->ip);
					I_Writer.tbsExport();
				}break;
				case IDC_SKINEXP:
				{
					I_Writer.SetBindPose(true);
					I_Writer.Initialize(tbsUtilExp::GetInstance()->ip);
					I_SkinExp.m_Scene = I_Writer.m_Scene;
					I_SkinExp.skmExport();
				}break;
				case IDC_MATRIXEXP2:
				{
					I_Writer.Initialize(tbsUtilExp::GetInstance()->ip);
					I_Writer.SetBindPose(true);
					I_Writer.matExport();
				}break;
			}break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			GetCOREInterface()->RollupMouseMessage(hWnd,msg,wParam,lParam);
			break;

		default:
			return 0;
	}
	return 1;
}
