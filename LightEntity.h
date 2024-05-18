#pragma once
#include"DebugRenderableEntity.h"

class LightEntity: public DebugRenderableEntity
{
public:
	LightEntity();
	~LightEntity();
	virtual void DebugRender(DX12Commandlist& cmdlist)override;

private:
	
};

