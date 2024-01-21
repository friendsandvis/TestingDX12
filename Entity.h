#pragma once
#include<string>
#include"DXUtils.h"

enum class ENTITYTYPE
{
	BASICRENDERABLEENTITY,
	GENERIC
};
class Entity
{
public:
	Entity();
	~Entity();
	std::string GetName() { m_name; }
	void SetName(std::string name) { m_name = name; }
	ENTITYTYPE GetEntityType() { return m_entityType; }
protected:
	std::string m_name;
	DirectX::XMMATRIX m_translatemat, m_rotatemat, m_scalemat;
	ENTITYTYPE m_entityType;

};