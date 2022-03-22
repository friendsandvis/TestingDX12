#include"DXCamera.h"

DXCamera::DXCamera()
{
	m_view = XMMatrixIdentity();
	m_projection = XMMatrixIdentity();
	m_model = XMMatrixIdentity();
}

DXCamera::~DXCamera()
{
}

XMMATRIX DXCamera::GetMVP()
{
	XMMATRIX mvp = XMMatrixMultiply(m_model, m_view);
	mvp = XMMatrixMultiply(mvp, m_projection);
	return mvp;
}