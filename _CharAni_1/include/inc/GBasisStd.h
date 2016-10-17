#ifndef GBASIS_H
#define GBASIS_H
#include "GUtils.h"
#include "GDxHelper.h"

#pragma comment( lib, "winmm.lib" )

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "GCoreLib32d.lib" )
#else
#pragma comment( lib, "GCoreLib32.lib" )
#endif
namespace GBASIS
{
	extern float		g_fSecPerFrame;
	extern float		g_fDurationTime;
	extern HWND			g_hWnd;
	extern HINSTANCE	g_hInstance;
	extern UINT			g_iClientWidth;
	extern UINT			g_iClientHeight;
}
using namespace GBASIS;
//////////////////////////////////////////////
// WinMain ∏≈≈©∑Œ
//////////////////////////////////////////////
#define GBASIS_START int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow ){ Sample gSample;
#define GBASIS_WIN(x) if( !gSample.InitWindow( hInstance, nCmdShow,  L#x ) )	{	return 0;	}	ShowCursor ( TRUE );gSample.Run();	return 1; }
#define GBASIS_RUN(x) GBASIS_START; GBASIS_WIN(x);

#endif //GBASIS_H