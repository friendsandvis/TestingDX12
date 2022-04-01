#include"CameraMovementManager.h"

DXFPSCameraController::DXFPSCameraController()
	:m_cameratocontrol(nullptr),
	m_moveforward(false)
{}
DXFPSCameraController::~DXFPSCameraController()
{}

void DXFPSCameraController::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	if (m_cameratocontrol == nullptr)
	{
		//nothing to manage so return
		return;
	}
	switch (uMsg)
	{

	case WM_MOUSEWHEEL:
	{
		float wheeldelta = GET_WHEEL_DELTA_WPARAM(wParam) / 30.0f;
		float fovupdated = m_cameratocontrol->GetFoV() - wheeldelta;
		m_cameratocontrol->SetFov(fovupdated);

	}
	case WM_XBUTTONDOWN:
	{
		m_moveforward = true; break;
	}
	case WM_XBUTTONUP:
		m_moveforward = false; break;
	default:
		break;
	}
}

void DXFPSCameraController::Update()
{
	if (m_cameratocontrol == nullptr)
	{
		//nothing to manage so return
		return;
	}
	if (m_moveforward)
	 {
		XMVECTOR camforward=m_cameratocontrol->GetCamForward();
		camforward = XMVector3Normalize(camforward);
		XMVECTOR campos = m_cameratocontrol->GetCamPos();
		campos = campos + camforward;
		m_cameratocontrol->SetCamPos(campos);	
	}
}
	