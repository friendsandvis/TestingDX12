#include"LightEntity.h"

LightEntity::LightEntity()
	:DebugRenderableEntity(MODELTYPE::BASIC_CUBE)
{
	m_entityType = ENTITYTYPE::UTILENTITY;
	BasicMaterialData basicMatData;
	basicMatData.m_albedo = { 0.0f,0.0f,1.0f,1.0f };
	SetBasicMaterialData(basicMatData);
}

LightEntity::~LightEntity()
{
}
void LightEntity::DebugRender(DX12Commandlist& cmdlist)
{
	BasicRenderableEntity::Render(cmdlist);
}