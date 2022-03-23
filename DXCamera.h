#pragma once
#include<DirectXMath.h>
using namespace DirectX;
class DXCamera
{
public:
	DXCamera();
	~DXCamera();
	XMMATRIX GetMVP();
	void SetProjection(XMMATRIX& aprojectionmat) { m_projection=aprojectionmat; }
	void SetView(XMMATRIX& aviewmat) { m_view = aviewmat; }
	void SetModel(XMMATRIX& amodelmat) { m_model = amodelmat; }
	inline const XMMATRIX& GetModel() { return m_model; }
	inline const XMMATRIX& GetView() { return m_view; }
	inline const XMMATRIX& GetProjection() { return m_projection; }
	inline float GetFoV() { return m_fovdegree; }
	void SetFov(float fovdeg) { m_fovdegree = fovdeg;  if (fovdeg >= 90.0f) { m_fovdegree - 45.0f; } }
private:
	XMMATRIX m_view;
	XMMATRIX m_model;
	XMMATRIX m_projection;
	float m_fovdegree;
};

