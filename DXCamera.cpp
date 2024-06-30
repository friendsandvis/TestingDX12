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
	m_camerapos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);//{ 0.0f,0.0f,3.0f };
	m_cameraTargetpos = XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f);
	m_up = {0.0f,1.0f,0.0f};
	m_forward = m_cameraTargetpos - m_camerapos;//XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);//{ 0.0f,0.0f,-1.0f };
	m_forward = XMVector3Normalize(m_forward);
	m_right = XMVector3Cross(m_up, m_forward);
	
	
}

DXCamera::~DXCamera()
{
}

XMMATRIX DXCamera::GetMVP(bool ortho, bool skipview)
{
	//update view matrix & projection matrix first
	GetView();
	GetProjection(ortho);
	XMMATRIX mvp=m_model;
	if (!skipview)
	{
		mvp= XMMatrixMultiply(m_model, m_view);
	}
	mvp = XMMatrixMultiply(mvp, m_projection);
	return mvp;
}
XMMATRIX DXCamera::GetVP(bool ortho)
{
	//update view matrix & projection matrix first
	GetView();
	GetProjection(ortho);
	XMMATRIX mvp = m_view;
	mvp = XMMatrixMultiply(mvp, m_projection);
	return mvp;
}

XMMATRIX DXCamera::GetView(bool update)
{
	if (update)
	{
		m_view = XMMatrixLookAtLH(m_camerapos, m_camerapos + m_forward, m_up);
		//m_view = XMMatrixLookAtLH(m_camerapos, m_cameraTargetpos, m_up);
	}
	return m_view;
}

XMMATRIX DXCamera::GetProjection(bool ortho)
{
	assert(m_aspectratio != 0.0f);
	if (!ortho)
	{
		 m_projection= XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fovdegree), m_aspectratio, m_near, m_far);
		
	}
	else
	{
		m_projection = XMMatrixOrthographicLH(m_viewwidth, m_viewheight, m_near, m_far);
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
#ifdef UPDATECAMERAVECTORTECHNIQUE_UPDATETARGETPOSITION
	XMVECTOR camToTargetVec = m_cameraTargetpos - m_camerapos;
	float camToTargetVecDistance = 0.0f;
	XMVECTOR camToTargetVecDistanceVec = XMVector3Length(camToTargetVec);
	XMStoreFloat(&camToTargetVecDistance, camToTargetVecDistanceVec);
	camToTargetVec = XMVector3Normalize(camToTargetVec);
	XMMATRIX xaxisrotation = XMMatrixRotationX(pitchinrads);
	camToTargetVec = XMVector4Transform(camToTargetVec, xaxisrotation);
	XMMATRIX yaxisrotation = XMMatrixRotationY(yawinrads);
	camToTargetVec = XMVector4Transform(camToTargetVec, yaxisrotation);
	XMVECTOR camToTargetVecResultant = camToTargetVec * camToTargetVecDistance;
	m_cameraTargetpos = m_camerapos + camToTargetVecResultant;
	m_forward = XMVector3Normalize(m_cameraTargetpos - m_camerapos);
	m_right = XMVector3Cross(m_up, m_forward);
#else
	double x = cos(yawinrads) * cos(pitchinrads);
	double y = sin(pitchinrads);
	double z = sin(yawinrads) * cos(pitchinrads);
	XMVECTOR newforward = XMVectorSet(x, y, z, 0.0f);
	newforward = XMVector3Normalize(newforward);
	m_forward = newforward;
	m_right = XMVector3Cross(m_up, m_forward);
#endif // UPDATECAMERAVECTORTECHNIQUE_UPDATETARGETPOSITION
}
void DXCamera::SetCamPos(XMVECTOR campos)
{
	m_camerapos = campos;
	//m_forward = m_cameraTargetpos - m_camerapos;
	//m_forward = XMVector3Normalize(m_forward);
	//m_right = XMVector3Cross(m_up, m_forward);
}