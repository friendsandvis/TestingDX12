#pragma once
#include"Entity.h"

class RenderableEntity:public Entity
{
public:
	RenderableEntity();
	~RenderableEntity();
	bool IsBasic() { return m_basic; }
private:
	bool m_basic;
};

