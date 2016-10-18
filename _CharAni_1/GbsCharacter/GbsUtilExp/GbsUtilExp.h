#pragma once
//$(MSBuildProgramFiles32)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.0
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

#include "3dsmaxsdk_preinclude.h"
#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
//SIMPLE TYPE
#include <utilapi.h>
// �߰�
#include <max.h>
#include "stdmat.h"
#include "decomp.h"
//#include "interpik.h"
#include "bipexp.h"
#include "phyexp.h"
#include "iskin.h"
#include <polyobj.h>
#include <Dummy.h>//DummyObject


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;