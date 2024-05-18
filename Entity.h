#pragma once
#include<string>
#include"DXUtils.h"
class SceneSerializer;
enum class ENTITYTYPE : unsigned
{
	BASICRENDERABLEENTITY = 0,
	GENERIC = 1,
	UTILENTITY = 2,
};
class Entity
{
public:
	Entity();
	virtual ~Entity();
	std::string GetName() { m_name; }
	void SetName(std::string name) { m_name = name; }
	void SetTransformationData(DirectX::XMFLOAT4 scale, DirectX::XMFLOAT4 rotationaxis,float rotationangle, DirectX::XMFLOAT4 translate);
	void SetTransformationData(float constantscale, DirectX::XMFLOAT4 rotationaxis, float rotationangle, DirectX::XMFLOAT4 translate);
	DirectX::XMMATRIX GetModelMatrix();
	ENTITYTYPE GetEntityType() { return m_entityType; }
protected:
	
	std::string m_name;
	DirectX::XMFLOAT4 m_translate;
	DirectX::XMFLOAT4 m_scale;
	DirectX::XMFLOAT4 m_rotationaxis;
	float m_rotationangle;
	ENTITYTYPE m_entityType;
	friend class SceneSerializer;
	

};