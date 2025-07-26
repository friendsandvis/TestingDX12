#include"CameraMovementManager.h"
#include<windowsx.h>
#include<WinUser.h>

DXFPSCameraController::DXFPSCameraController()
	:m_cameratocontrol(nullptr),
	m_movefrontbackmodifier(0.0f),
	m_moveleftrightmodifier(0.0f),
	m_mousefirstmove(true),
	m_lastmouseXpos(0),
	m_lastmouseYpos(0),
	m_campitch(0.0f),

	m_camyaw(0.0f),
	m_allowMouseMevementProcessing(true)
{
#ifndef UPDATECAMERAVECTORTECHNIQUE_UPDATETARGETPOSITION
	m_camyaw = -90.0f;
#endif
}
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

		if ((xbutton == XBUTTON1))
		{
			m_movefrontbackmodifier = -1.0f;
		}
		else
		{
			m_movefrontbackmodifier = 1.0f;
		}

		break;
	}
	case WM_KEYDOWN:
	{
		
		switch (wParam)
		{
		case VK_UP:
		{m_movefrontbackmodifier = 1.0f;
		break;
		}
		case VK_DOWN:
		{m_movefrontbackmodifier = -1.0f;
		break;
		}
		case VK_LEFT:
		{m_moveleftrightmodifier = -1.0f;
		break;
		}
		case VK_RIGHT:
		{m_moveleftrightmodifier = 1.0f;
		break;
		}
		default:
			break;
		}

		
		break;
	}
	case WM_KEYUP:
	case WM_XBUTTONUP:
	{
		m_moveleftrightmodifier = 0.0f;
		m_movefrontbackmodifier = 0.0f; break;
	}
	default:
		break;
	case WM_MOUSEMOVE:
	{
		if (m_allowMouseMevementProcessing)
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
			float yawoffset = sensitivity * xoffset;
			float pitchoffset = sensitivity * yoffset;
#ifdef UPDATECAMERAVECTORTECHNIQUE_UPDATETARGETPOSITION
			m_cameratocontrol->UpdateCameraVectors(pitchoffset, yawoffset);
#else
			m_camyaw -= sensitivity * xoffset;
			m_campitch += sensitivity * yoffset;
			m_cameratocontrol->UpdateCameraVectors(m_campitch, m_camyaw);
#endif // UPDATECAMERAVECTORTECHNIQUE_UPDATETARGETPOSITION
		}
		
		
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
		campos = campos + 0.01f* m_movefrontbackmodifier*camforward;
		m_cameratocontrol->SetCamPos(campos);
		XMVECTOR camTargetpos = m_cameratocontrol->GetCamTargetPos();
		camTargetpos = camTargetpos + 0.01f * m_movefrontbackmodifier * camforward;
		m_cameratocontrol->SetCamTargetPos(camTargetpos);
	}
	if (m_moveleftrightmodifier != 0.0f)
	{
		XMVECTOR camright = m_cameratocontrol->GetCamRight();
		XMVECTOR campos = m_cameratocontrol->GetCamPos();
		campos = campos + 0.01f * m_moveleftrightmodifier * camright;
		m_cameratocontrol->SetCamPos(campos);
		//update target vector as well when moving leftright to keep the forward vvector same;else there could be a bug upon first mouse(rotation) movement after leftright movewment in some cases
		XMVECTOR camTargetpos = m_cameratocontrol->GetCamTargetPos();
		camTargetpos = camTargetpos + 0.01f * m_moveleftrightmodifier * camright;
		m_cameratocontrol->SetCamTargetPos(camTargetpos);

	}
}
	