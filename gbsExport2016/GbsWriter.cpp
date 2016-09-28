#include "GbsWriter.h"

struct GScene
{
	int iFirstFrame;
	int iLastFrame;
	int iFrameSpeed;
	int iTickPerFrame;
};

void GbsWriter::Init(TSTR name,
	Interface* p3dsMax) {

	m_p3dsMax = p3dsMax;
	m_szFileName = name;

	m_fp = _tfopen(m_szFileName, _T("wt"));
	_ftprintf(m_fp, _T("%s\n"), _T("GBS_FILE_EXPORT"));
};
void GbsWriter::Release() { fclose(m_fp); };
bool GbsWriter::Exporter() {
	
	ExpScene();
	ExpMaterial();
	ExpObject();
	return true; 
};
bool GbsWriter::ExpScene() {
	
	Interval m_Interval = m_p3dsMax->GetAnimRange();
	GScene scene;
	scene.iTickPerFrame = GetTicksPerFrame();
	scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();
	scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();
	scene.iFrameSpeed = GetFrameRate();
	_ftprintf(m_fp, _T("%s %d %d %d %d\n"),
		_T("#SCENE"),
		scene.iFirstFrame,
		scene.iLastFrame,
		scene.iFrameSpeed,
		scene.iTickPerFrame);
		
	return true; 
};
bool GbsWriter::ExpMaterial() {
	
	return true; 
};
bool GbsWriter::ExpObject() {
	
	return true; 
};

GbsWriter::GbsWriter()
{
}


GbsWriter::~GbsWriter()
{
}
