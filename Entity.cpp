#include"Entity.h"

Entity::Entity()
	:m_name("notspecified"),
	m_entityType(ENTITYTYPE::GENERIC),
	m_scale(1.0f,1.0f, 1.0f, 1.0f),
	m_translate(0.0f, 0.0f, 0.0f, 0.0f),
	m_rotationaxis(0.0f, 0.0f, 0.0f, 0.0f),
	m_rotationangle(0.0f)
{
}

Entity::~Entity()
{
}
void Entity::SetTransformationData(DirectX::XMFLOAT4 scale, DirectX::XMFLOAT4 rotationaxis, float rotationangle, DirectX::XMFLOAT4 translate)
{
	m_scale = scale;
	m_rotationaxis = rotationaxis;
	m_rotationangle = rotationangle;
	m_translate = translate;
}
void Entity::SetTransformationData(float constantscale, DirectX::XMFLOAT4 rotationaxis, float rotationangle, DirectX::XMFLOAT4 translate)
{
	m_scale = DirectX::XMFLOAT4(constantscale, constantscale, constantscale, constantscale);
	m_rotationaxis = rotationaxis;
	m_rotationangle = rotationangle;
	m_translate = translate;
}
DirectX::XMMATRIX Entity::GetModelMatrix()
{
	return DXUtils::GetTransformationMatrix(DXUtils::Float4tovector(m_scale), DXUtils::Float4tovector(m_rotationaxis), m_rotationangle, DXUtils::Float4tovector(m_translate) );
}