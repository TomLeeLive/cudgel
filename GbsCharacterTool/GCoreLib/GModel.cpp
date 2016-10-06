#include "GModel.h"

GModel::GModel(void)
{
	D3DXMatrixIdentity(&m_matControlWorld);
	m_pd3dDevice = NULL;
	m_fElapseTime = 0.0f;
	m_iStartFrame = 0;
	m_iLastFrame = 0;
	m_bSetLoadDraw = true;
	m_iMaxVertex = 0;
	m_iMaxIndex = 0;
	m_iBeginPosVB = 0;
	m_iBeginPosIB = 0;
}

GModel::~GModel(void)
{

}
