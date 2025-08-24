#pragma once
#include"DXUtils.h"
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
#include"CameraMovementManager.h"
using namespace DirectX;
class ModelTestApplication :public DX12ApplicationManagerBase
{
public:
	ModelTestApplication();
	~ModelTestApplication();
	void PreRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	struct GeneralConstants
	{
		unsigned int usematerialtextures;
	};
	DX12PSO m_pso,m_pso_alphablending;
	Model m_planemodel,m_trianglemodel;
	Model m_cubemodel;
	Model m_loadedmodel;
	CompoundModel m_loadedcompoundmodel;
	DXFPSCameraController m_maincameracontroller;
	void InitPSO();
};