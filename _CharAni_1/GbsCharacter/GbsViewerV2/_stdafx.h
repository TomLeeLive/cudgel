#pragma once

#ifndef __STDAFX_H__
#define __STDAFX_H__

#include <GBasisLib_0.h>
#include <GShape.h>
#include <GParser.h>

enum G_TEX_SUPPORT {
	G_TEX_DIFFUSE =1,
	G_TEX_REFLECT =9,
};
//const enum OBJECTCLASSTYPE {
//	CLASS_GEOM = 0,
//	CLASS_BONE,
//	CLASS_DUMMY,
//	CLASS_BIPED,
//};

#include "GGbsParser.h"
#include "GGbsModel.h"
#include "GModelViewCamera.h"
#include "GbsViewer.h"
#endif