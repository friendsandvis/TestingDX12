#pragma once
#include"BasicRenderableEntity.h"
#include"EntityRenderer.h"

class BasicEntityRenderer :public EntityRenderer
{
public:
	BasicEntityRenderer();
	~BasicEntityRenderer();
	void Render() override;
private:

};

