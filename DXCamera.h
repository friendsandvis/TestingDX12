#pragma once
#include<DirectXMath.h>
using namespace DirectX;
// perspectice projection camera
class DXCamera
{
public:
	DXCamera();
	~DXCamera();
	XMMATRIX GetMVP();
	void SetProjection(XMMATRIX& aprojectionmat) { m_projection=aprojectionmat; }
	void SetModel(XMMATRIX& amodelmat) { m_model = amodelmat; }
	inline const XMMATRIX& GetModel() { return m_model; }
	XMMATRIX GetView(bool update=true);
	XMMATRIX GetProjection(bool update = true);
	inline float GetFoV() { return m_fovdegree; }
	void SetFov(float fovdeg);
	float GetAspectRatio() { return m_aspectratio; }
	void SetAspectRatio(float ar) { m_aspectratio=ar; }
	XMVECTOR GetCamPos() { return m_camerapos; }
	XMVECTOR GetCamUp() { return m_up; }
	XMVECTOR GetCamForward() { return m_forward; }
	void SetCamPos(XMVECTOR campos) { m_camerapos=campos; }
	void UpdateCameraVectors(float pitch, float yaw);
private:
	XMVECTOR m_camerapos;
	XMVECTOR m_up, m_forward;
	XMMATRIX m_model;
	XMMATRIX m_view;
	XMMATRIX m_projection;
	float m_fovdegree,m_aspectratio,m_near,m_far;
};

