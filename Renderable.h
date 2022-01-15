#pragma once

class Renderable
{
public:
	Renderable() {}
	~Renderable() {}

	virtual void Render() = 0;

};
