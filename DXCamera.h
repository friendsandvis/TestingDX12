#pragma once
//#define UPDATECAMERAVECTORTECHNIQUE_UPDATETARGETPOSITION
/*
* two techniques used to update camera vectors with Pitch and yaw angles.
* 1. old does not update target position where pitch and yaw vector are used to calculate forwardvector directly by using sin & cos(target position is ignored)
* 2. new where pitch & yaw are used to rotate previous camera vectors along CX & Y axis,target position is updated. 
*/
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
	XMVECTOR GetCamTargetPos() { return m_cameraTargetpos; }
	void SetCamPos(XMVECTOR campos);
	void UpdateCameraVectors(float pitch, float yaw);
	void SetCamTargetPos(XMVECTOR targetpos) { m_cameraTargetpos = targetpos; }
private:
	XMVECTOR m_camerapos;
	XMVECTOR m_cameraTargetpos;
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

