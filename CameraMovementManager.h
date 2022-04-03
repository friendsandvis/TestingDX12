#pragma once
#include"DXCamera.h"
#include<Windows.h>

//this class controls the movement of camera in fps camera style
class DXFPSCameraController
{
public:
	DXFPSCameraController();
	~DXFPSCameraController();
	void SetCameratoControl(DXCamera* cameratocontrol) { m_cameratocontrol = cameratocontrol; }
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Update();
	bool isMovingForward() { return m_moveforward; }
private:
	DXCamera* m_cameratocontrol;
	bool m_moveforward;
	//requirnments to control camera movement via mouse
	float yaw, pitch;
};
