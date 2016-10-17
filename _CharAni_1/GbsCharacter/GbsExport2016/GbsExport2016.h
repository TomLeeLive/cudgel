#pragma once

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
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/
//#ifdef _UNICODE
//	#undef _UNICODE
//#endif
#include "3dsmaxsdk_preinclude.h"
#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
//SIMPLE TYPE
#include <impexp.h>
#include <max.h>
#include <StdMat.h>
#include <decomp.h>

// 위자드로 생성된 dll 프로젝트는 release로 변경해야 한다.
//C:\Program Files\Autodesk\3ds Max 2016 SDK\maxsdk\lib\x64\Debug\
//C:\Program Files\Autodesk\3ds Max 2016 SDK\maxsdk\lib\x64\Release\

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
