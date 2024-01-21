#pragma once
#include"Entity.h"
#include"DX12CommandList.h"


class RenderableEntity:public Entity
{
public:
	RenderableEntity();
	~RenderableEntity();
	bool IsBasicModel() { return m_basic; }
	virtual void Init(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& cmdlist)=0;
	virtual void Render(DX12Commandlist& cmdlist)=0;
private:
	bool m_basic;

};

