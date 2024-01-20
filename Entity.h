#pragma once
#include<string>
#include"DXUtils.h"


class Entity
{
public:
	Entity();
	~Entity();
	std::string GetName() { m_name; }
	void SetName(std::string name) { m_name = name; }
private:
	std::string m_name;
	DirectX::XMMATRIX m_translatemat, m_rotatemat, scalemat;

};