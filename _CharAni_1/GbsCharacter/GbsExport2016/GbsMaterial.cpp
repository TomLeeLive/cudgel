#include "GbsMaterial.h"

void TbsMaterial::GetMaterial()
{
	for( int iMtl= 0 ; iMtl < m_pMtlList.size() ;iMtl++ )
	{
		Mtl* pSrcMtl = m_pMtlList[iMtl];
		
		TMtl AddMtl;	
		TSTR className;
		pSrcMtl->GetClassName(className);
		AddMtl.strName		= FixupName(pSrcMtl->GetName());
		AddMtl.strClassName = FixupName(className);
		AddMtl.iNumSubMtl= pSrcMtl->NumSubMtls();
		AddMtl.iIndex	= iMtl;	

		if( pSrcMtl->NumSubMtls() > 0) 
		{			
			for ( int iSubMtrl=0; iSubMtrl< pSrcMtl->NumSubMtls(); iSubMtrl++) 
			{
				Mtl* subMtl = pSrcMtl->GetSubMtl(iSubMtrl);
				
				TMtl AddSubMtl;
				AddSubMtl.iIndex	= iSubMtrl;
				AddSubMtl.iNumSubMtl= subMtl->NumSubMtls();
				AddSubMtl.strName	= FixupName(subMtl->GetName());	

				TSTR className;
				subMtl->GetClassName(className);
				AddSubMtl.strClassName	= FixupName(className);

				if(subMtl->NumSubMtls()> 0)
				{
					MessageBox(0,_T("다중 서브 매터리얼"),_T("Fatal error"), MB_OK );
				}
				if (subMtl) 
				{
					GetTexture( &AddSubMtl, subMtl);							
				}
				AddMtl.SubMaterial.push_back(AddSubMtl);
			}
		}
		else
		{	

			GetTexture( &AddMtl, pSrcMtl);
		}		
		m_Material.push_back(AddMtl);
	}
	
			
}
void TbsMaterial::GetTexture(TMtl* DestMtrl, Mtl* pSrcMtl )
{
	for ( int iSubMap =0; iSubMap<pSrcMtl->NumSubTexmaps(); iSubMap++) 
	{
		Texmap* tex = pSrcMtl->GetSubTexmap(iSubMap);			
		if (tex) 
		{	
			// If it is a standard material we can see if the map is enabled.
			if (pSrcMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) 
			{
				StdMat* std = (StdMat*)pSrcMtl;
				if (!std->MapEnabled(iSubMap))
					continue;								
			}
			
			TTexMap TexMap;
			TexMap.iType = iSubMap;

			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) 
			{
				TSTR mapName = ((BitmapTex *)tex)->GetMapName();	
				TSTR strFullName;
				SplitPathFile( mapName,&strFullName,&mapName);
				TexMap.strTextureName = FixupName(mapName);
			}
			DestMtrl->TexMaps.push_back(TexMap);					
			DestMtrl->iNumMap = DestMtrl->TexMaps.size();
		}
	}	
}

TbsMaterial::TbsMaterial(void)
{
}

TbsMaterial::~TbsMaterial(void)
{
}
