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
private:
	DXCamera* m_cameratocontrol;
	bool m_moveforward;
};
