#pragma once
#include<DirectXMath.h>
using namespace DirectX;
// perspectice projection camera
class DXCamera
{
public:
	DXCamera();
	~DXCamera();
	XMMATRIX GetMVP(bool ortho=false,bool skipview=false);
	XMMATRIX GetVP(bool ortho = false);
	void SetProjection(XMMATRIX& aprojectionmat) { m_projection=aprojectionmat; }
	void SetModel(XMMATRIX& amodelmat) { m_model = amodelmat; }
	inline const XMMATRIX& GetModel() { return m_model; }
	XMMATRIX GetView(bool update=true);
	XMMATRIX GetProjection(bool ortho = false);
	inline float GetFoV() { return m_fovdegree; }
	void SetFov(float fovdeg);
	float GetAspectRatio() { return m_aspectratio; }
	void SetAspectRatio(unsigned int viewwidth,unsigned int viewheight)
	{
		m_viewwidth = viewwidth;
		m_viewheight = viewheight;
		m_aspectratio = ((float)viewwidth) / viewheight;
	}
	XMVECTOR GetCamPos() { return m_camerapos; }
	XMVECTOR GetCamUp() { return m_up; }
	XMVECTOR GetCamForward() { return m_forward; }
	XMVECTOR GetCamRight() { return m_right; }
	void SetCamPos(XMVECTOR campos) { m_camerapos=campos; }
	void UpdateCameraVectors(float pitch, float yaw);
private:
	XMVECTOR m_camerapos;
	XMVECTOR m_up, m_forward,m_right;
	XMMATRIX m_model;
	XMMATRIX m_view;
	XMMATRIX m_projection;
	float m_fovdegree,m_aspectratio,m_near,m_far;
	unsigned int m_viewwidth, m_viewheight;
};

struct CameraConstants
{
	XMMATRIX mvp;
	XMVECTOR campos;
};

