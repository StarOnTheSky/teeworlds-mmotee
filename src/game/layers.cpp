/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "layers.h"
#include "gamecore.h"

CLayers::CLayers()
{
	m_GroupsNum = 0;
	m_GroupsStart = 0;
	m_LayersNum = 0;
	m_LayersStart = 0;
	m_pGameGroup = 0;
	m_pPhysicsLayer = 0;
	m_pZoneGroup = 0;
	m_pEntityGroup = 0;
	m_pMap = 0;
}

void CLayers::Init(class IKernel *pKernel)
{
	m_pMap = pKernel->RequestInterface<IMap>();
	m_pMap->GetType(MAPITEMTYPE_GROUP, &m_GroupsStart, &m_GroupsNum);
	m_pMap->GetType(MAPITEMTYPE_LAYER, &m_LayersStart, &m_LayersNum);

	for(int g = 0; g < NumGroups(); g++)
	{
		CMapItemGroup *pGroup = GetGroup(g);
		
		char aGroupName[12];
		IntsToStr(pGroup->m_aName, sizeof(aGroupName)/sizeof(int), aGroupName);
		
		if(str_comp(aGroupName, "#Zones") == 0)
			m_pZoneGroup = pGroup;
		else if(str_comp(aGroupName, "#Entities") == 0)
			m_pEntityGroup = pGroup;
		else
		{
			for(int l = 0; l < pGroup->m_NumLayers; l++)
			{
				CMapItemLayer *pLayer = GetLayer(pGroup->m_StartLayer+l);

				if(pLayer->m_Type == LAYERTYPE_TILES)
				{
					CMapItemLayerTilemap *pTilemap = reinterpret_cast<CMapItemLayerTilemap *>(pLayer);
					if(pTilemap->m_Flags&TILESLAYERFLAG_PHYSICS)
					{
						m_pPhysicsLayer = pTilemap;
						m_pGameGroup = pGroup;

						// make sure the game group has standard settings
						m_pGameGroup->m_OffsetX = 0;
						m_pGameGroup->m_OffsetY = 0;
						m_pGameGroup->m_ParallaxX = 100;
						m_pGameGroup->m_ParallaxY = 100;

						if(m_pGameGroup->m_Version >= 2)
						{
							m_pGameGroup->m_UseClipping = 0;
							m_pGameGroup->m_ClipX = 0;
							m_pGameGroup->m_ClipY = 0;
							m_pGameGroup->m_ClipW = 0;
							m_pGameGroup->m_ClipH = 0;
						}
					}
				}
			}
		}
	}
	
	if(!m_pPhysicsLayer)
		dbg_msg("mmotee", "CLayer::Init: no Game Layer found");
}

CMapItemGroup *CLayers::GetGroup(int Index) const
{
	return static_cast<CMapItemGroup *>(m_pMap->GetItem(m_GroupsStart+Index, 0, 0));
}

CMapItemLayer *CLayers::GetLayer(int Index) const
{
	return static_cast<CMapItemLayer *>(m_pMap->GetItem(m_LayersStart+Index, 0, 0));
}
