#include"BasicEntityRenderer.h"

BasicEntityRenderer::BasicEntityRenderer()
{
}

BasicEntityRenderer::~BasicEntityRenderer()
{
}
void BasicEntityRenderer::Render()
{
	for (std::shared_ptr<Entity> entity : m_entityList)
	{
		BasicRenderableEntity* basicRenderableEntity=dynamic_cast<BasicRenderableEntity*>(entity.get());
		
	}
}