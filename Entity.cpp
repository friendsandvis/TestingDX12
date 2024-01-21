#include"Entity.h"

Entity::Entity()
	:m_name("notspecified"),
	m_entityType(ENTITYTYPE::GENERIC),
	m_translatemat(DirectX::XMMatrixIdentity()),
	m_rotatemat(DirectX::XMMatrixIdentity()),
	m_scalemat(DirectX::XMMatrixIdentity())
{
}

Entity::~Entity()
{
}