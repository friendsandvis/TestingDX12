#include"CameraMovementManager.h"
#include<windowsx.h>
#include<WinUser.h>

DXFPSCameraController::DXFPSCameraController()
	:m_cameratocontrol(nullptr),
	m_movefrontbackmodifier(0.0f),
	m_mousefirstmove(true),
	m_lastmouseXpos(0),
	m_lastmouseYpos(0),
	m_campitch(0.0f),
	m_camyaw(-90.0f)
	
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
		float wheeldelta = GET_WHEEL_DELTA_WPARAM(wParam) / 100.0f;
		float fovupdated = m_cameratocontrol->GetFoV() - wheeldelta;
		m_cameratocontrol->SetFov(fovupdated);
		break;
	}
	case WM_XBUTTONDOWN:
	{
		WORD xbutton=GET_XBUTTON_WPARAM(wParam);

		if (xbutton == XBUTTON1)
		{
			m_movefrontbackmodifier = -1.0f;
		}
		else
		{
			m_movefrontbackmodifier = 1.0f;
		}

		break;
	}
	case WM_XBUTTONUP:
		m_movefrontbackmodifier = 0.0f; break;
	default:
		break;
	case WM_MOUSEMOVE:
	{
		int xpos = GET_X_LPARAM(lParam);
		int ypos = GET_Y_LPARAM(lParam);
		if (m_mousefirstmove)
		{
			m_lastmouseXpos = xpos;
			m_lastmouseYpos = ypos;
			m_mousefirstmove = false;
		}
		float sensitivity = 0.1f;
		int xoffset = xpos - m_lastmouseXpos;
		int yoffset = m_lastmouseYpos - ypos;
		m_lastmouseXpos = xpos;
		m_lastmouseYpos = ypos;
		m_camyaw -= sensitivity*xoffset;
		m_campitch += sensitivity*yoffset;
		m_cameratocontrol->UpdateCameraVectors(m_campitch, m_camyaw);
		break;
	}
	}
}

void DXFPSCameraController::Update()
{
	if (m_cameratocontrol == nullptr)
	{
		//nothing to manage so return
		return;
	}
	if (m_movefrontbackmodifier!=0.0f)
	 {
		XMVECTOR camforward=m_cameratocontrol->GetCamForward();
		camforward = XMVector3Normalize(camforward);
		XMVECTOR campos = m_cameratocontrol->GetCamPos();
		campos = campos + 0.25f* m_movefrontbackmodifier*camforward;
		m_cameratocontrol->SetCamPos(campos);	
	}
}
	