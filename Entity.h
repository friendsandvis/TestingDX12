#pragma once
#include<string>
#include"DXUtils.h"


class Entity
{
public:
	Entity();
	~Entity();

private:
	std::string m_name;
	DirectX::XMMATRIX m_translatemat, m_rotatemat, scalemat;

};