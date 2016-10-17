#pragma once
#include "GWindow.h"
#include "GTimer.h"
#include "GInput.h"
#include "GDirectWrite.h"
#include "GShape.h"

class GBasisLib_0 : public GWindow
{
public:
	GTimer					m_Timer;
	GDirectWrite			m_Font;
	UINT					m_iPrimitiveType;
	UINT					m_iCullMode;
	UINT					m_iSamplerMode;
	bool					m_bWireFrameRender;
	bool					m_bDebugPrint;
	GDirectionLineShape		m_AxisLine;
	T_STR					m_LineShaderFile;
public:
	virtual bool		Init();
	virtual bool		Frame();
	virtual bool		Render();
	virtual bool		Release();
	virtual int			WndProc( HWND, UINT, WPARAM, LPARAM );

	virtual bool		PreInit();	
	virtual bool		PostInit();	 
		
	virtual bool		PreFrame();		
	virtual bool		PostFrame();
	 
	virtual bool		Update(ID3D11DeviceContext*    pContext);
	virtual	bool		PreRender();
	virtual bool		DrawDebug();
	virtual bool		DrawDebugRect(RECT* rcDest = NULL, TCHAR* pString = 0, D3DXCOLOR color = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f) );
	virtual	bool		PostRender();

	bool				GInit();
	bool				GFrame();
	bool				GRender();	
	bool				Run();		 
	bool				ToolRun();
	bool				GRelease();

				HRESULT		CreateDxResource();
				HRESULT		DeleteDxResource();
	virtual		HRESULT		CreateResource();
	virtual		HRESULT		DeleteResource();
	
public:
	GBasisLib_0(void);
	virtual ~GBasisLib_0(void);
};
