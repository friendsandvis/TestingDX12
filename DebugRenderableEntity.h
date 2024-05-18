#pragma once
#include"BasicRenderableEntity.h"

class DebugRenderableEntity :public BasicRenderableEntity
{
public:
	DebugRenderableEntity();
	DebugRenderableEntity(MODELTYPE modeltype);
	~DebugRenderableEntity();
	virtual void Init(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& cmdlist) override;
	virtual void DebugRender(DX12Commandlist& cmdlist) = 0;
	virtual void Render(DX12Commandlist& cmdlist) override;
private:

};