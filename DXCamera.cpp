#include"DXCamera.h"

DXCamera::DXCamera()
	:
	m_fovdegree(45.0f)
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