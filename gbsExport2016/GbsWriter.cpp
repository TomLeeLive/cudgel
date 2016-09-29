#include "GbsWriter.h"



void GbsWriter::GetTexture(Mtl* pMtl, GMtl& gSubMtl) {
	GTex tex;
	tex.iMapType = 1;
	tex.szTexName = _T("null");
	gSubMtl.texlist.push_back(tex);

}
void GbsWriter::AddObject(INode* pNode) {
	ObjectState os = pNode->EvalWorldState(m_Interval.Start());
	Object *obj = os.obj;
	if (obj)
	{
		switch (obj->SuperClassID()) 
		{
			case GEOMOBJECT_CLASS_ID:
			case HELPER_CLASS_ID:
			{
				m_ObjectList.push_back(pNode);
			}break;
		}
	}
}
void GbsWriter::AddMtl(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl != NULL) {
		bool bAdd = true;
		for (int iCnt = 0; iCnt < m_MtlList.size(); iCnt++) {
			if (m_MtlList[iCnt] == pMtl) {
				bAdd = false;
				break;
			}
		}
		if (bAdd)  m_MtlList.push_back(pMtl);
	}
	
}
void GbsWriter::PreProcess(INode* pNode) {
	if (pNode == NULL) return;

	AddObject(pNode);
	AddMtl(pNode);

	int iNumChildren = pNode->NumberOfChildren();
	for (int iChild = 0; iChild < iNumChildren; iChild++) {
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);

	}

}

void GbsWriter::Init(TSTR name,
	Interface* p3dsMax) {

	m_p3dsMax = p3dsMax;
	m_szFileName = name;
	m_pRootNode = m_p3dsMax->GetRootNode();
	PreProcess(m_pRootNode);

	m_Scene.iVersion = 100;
	m_Scene.iNumMtl = m_MtlList.size();
	m_Scene.iNumObject = m_ObjectList.size();


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

	m_Scene.iTickPerFrame = GetTicksPerFrame();
	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();
	m_Scene.iFrameSpeed = GetFrameRate();
	_ftprintf(m_fp, _T("%s %d %d %d %d %d %d %d\n"),
		_T("#SCENE"),
		m_Scene.iVersion,
		m_Scene.iFirstFrame,
		m_Scene.iLastFrame,
		m_Scene.iFrameSpeed,
		m_Scene.iTickPerFrame,
		m_Scene.iNumMtl,
		m_Scene.iNumObject);	
	return true; 
};
bool GbsWriter::ExpMaterial() {
	
	for (int iMtl = 0; iMtl < m_MtlList.size(); iMtl++) {
		Mtl* pMtl = m_MtlList[iMtl];
		GMtl gMtl;
		gMtl.iNumSubMtls = pMtl->NumSubMtls();
		if (pMtl->NumSubMtls() > 0) {

			for (int iSubMtl = 0; iSubMtl < pMtl->NumSubMtls(); iSubMtl++) {
				GMtl gSubMtl;
				gSubMtl.iNumSubMtls = 0;
				GetTexture(pMtl, gSubMtl);
				gMtl.subMtls.push_back(gSubMtl);

			}
		}
		else {
			GetTexture(pMtl, gMtl);
		}
		m_gMtlList.push_back(gMtl);
	}





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
