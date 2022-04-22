#pragma once
#include"DXUtils.h"
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
#include"CameraMovementManager.h"
using namespace DirectX;
class RayTracingApplication :public DX12ApplicationManagerBase
{
public:
	RayTracingApplication();
	~RayTracingApplication();
	void PreRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	DX12PSO m_pso;
	DX12RootSignature m_rootsignature;
	Model m_cubemodel;
	DXFPSCameraController m_maincameracontroller;
	void InitPSO();
};