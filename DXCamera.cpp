#include"DXCamera.h"
#include<math.h>

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
	m_camerapos = { 0.0f,0.0f,3.0f };
	m_up = {0.0f,1.0f,0.0f};
	m_forward = { 0.0f,0.0f,-1.0f };
	
	
}

DXCamera::~DXCamera()
{
}

XMMATRIX DXCamera::GetMVP()
{
	//update view matrix & projection matrix first
	GetView();
	GetProjection();
	XMMATRIX mvp = XMMatrixMultiply(m_model,m_view);
	mvp = XMMatrixMultiply(mvp, m_projection);
	return mvp;
}

XMMATRIX DXCamera::GetView(bool update)
{
	if (update)
	{
		m_view = XMMatrixLookAtLH(m_camerapos, m_camerapos + m_forward, m_up);
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

void DXCamera::UpdateCameraVectors(float pitch, float yaw)
{
	double pitchinrads = (double)XMConvertToRadians(pitch);
	double yawinrads = (double)XMConvertToRadians(yaw);
	double x = cos(yawinrads) * cos(pitchinrads);
	double y = sin(pitchinrads);
	double z = sin(yawinrads)*cos(pitchinrads);
	XMVECTOR newforward = XMVectorSet(x, y, z,0.0f);
	newforward = XMVector3Normalize(newforward);
	m_forward = newforward;

}