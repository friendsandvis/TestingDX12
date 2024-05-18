#include"DebugRenderableEntity.h"

DebugRenderableEntity::DebugRenderableEntity()
	: BasicRenderableEntity(MODELTYPE::BASIC_CUBE)
{

}
DebugRenderableEntity::DebugRenderableEntity(MODELTYPE modeltype)
	: BasicRenderableEntity(modeltype)
{

}

DebugRenderableEntity::~DebugRenderableEntity()
{

}
void DebugRenderableEntity::Render(DX12Commandlist& cmdlist)
{
	DebugRender(cmdlist);
}
void DebugRenderableEntity::Init(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& cmdlist)
{
	BasicRenderableEntity::Init(creationdevice, cmdlist);
}
