#pragma once
#include"Entity.h"
#include<forward_list>
#include<memory>

class EntityRenderer
{
public:
	EntityRenderer();
	~EntityRenderer();
	inline void AddEntity(std::shared_ptr<Entity> entity)
	{
		m_entityList.push_front(entity);
	}
	virtual void Render() =0;
protected:
	std::forward_list<std::shared_ptr<Entity>> m_entityList;
};

EntityRenderer::EntityRenderer()
{
}

EntityRenderer::~EntityRenderer()
{
}
