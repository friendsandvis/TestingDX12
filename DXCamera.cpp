#include"DXCamera.h"

DXCamera::DXCamera()
	:
	m_fovdegree(45.0f),
	m_aspectratio(0.0f),
	m_near(0.1f),
	m_far(100.0f)
{
	m_view = XMMatrixIdentity();
	m_projection = XMMatrixIdentity();
	m_model = XMMatrixIdentity();
	m_camerapos = { 0.0f,0.0f,10.0f };
	m_targetpoint = {0.0f,0.0f,0.0f};
	m_up = {0.0f,1.0f,0.0f};
	m_forward = m_targetpoint - m_camerapos;
	
	
}

DXCamera::~DXCamera()
{
}

XMMATRIX DXCamera::GetMVP()
{
	//update view matrix & projection matrix first
	GetView();
	GetProjection();
	XMMATRIX mvp = XMMatrixMultiply(m_model, m_view);
	mvp = XMMatrixMultiply(mvp, m_projection);
	return mvp;
}

XMMATRIX DXCamera::GetView(bool update)
{
	if (update)
	{
		m_view = XMMatrixLookAtLH(m_camerapos, m_targetpoint, m_up);
	}
	return m_view;
}

XMMATRIX DXCamera::GetProjection(bool update)
{
	assert(m_aspectratio != 0.0f);
	if (update)
	{
		 m_projection= XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fovdegree), m_aspectratio, m_near, m_far);
	}
	return m_projection;
}

void DXCamera::SetFov(float fovdeg)
{
	if (fovdeg >= 90)
	{
		fovdeg -= 45.0f;
	}
	else if(fovdeg<=0.0f)
	{
		fovdeg += 45.0f;
	}
	m_fovdegree = fovdeg;
}