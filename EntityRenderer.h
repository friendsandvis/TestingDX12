#pragma once
#include"Entity.h"
#include<forward_list>
#include<memory>
#include"DX12CommandList.h"

class EntityRenderer
{
public:
	struct RenderData
	{
		DirectX::XMMATRIX vpmat;
	};
	EntityRenderer();
	~EntityRenderer();
	inline void AddEntity(std::shared_ptr<Entity> entity)
	{
		m_entityList.push_front(entity);
	}
	virtual void Init(ComPtr< ID3D12Device> creationdevice) = 0;
	virtual void Render(DX12Commandlist& cmdlist, RenderData& renderData) =0;
protected:
	std::forward_list<std::shared_ptr<Entity>> m_entityList;
};
