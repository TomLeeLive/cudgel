#pragma once
#include "GbsGlobal.h"

struct TTexMap
{
	int		iIndex;
	int		iType; 	// �ؽ��� Ÿ�� �ε���
	TSTR	strTextureName; 	// �ؽ��� �̸�.
};

struct TMtl
{	
	int				iIndex;
	int				iNumSubMtl;	// ���� ���͸��� ����	
	int				iNumMap;	// �ؽ��� �� ����
	TSTR			strName; // ���͸��� �̸�	
	TSTR			strClassName;// ��Ŭ���� �̸�
	vector<TTexMap>	TexMaps; // �� Ÿ�Ե�	
	vector<TMtl>	SubMaterial; // ���� ���͸��� ���庤��
};

class TbsMaterial : public TbsGlobal
{
public:				
		vector<TMtl>	m_Material;// ��� �� ���� ����Ʈ	
public:	
		void	GetMaterial();//���͸��� ����Ʈ���� ���� ���
		void	GetTexture( TMtl* pDestMtl, Mtl* pSrcMtl );//���͸��� ����Ʈ���� ���� ���
public:
	TbsMaterial(void);
	virtual~TbsMaterial(void);
};
