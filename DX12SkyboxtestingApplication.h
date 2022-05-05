#pragma once
#include"DXUtils.h"
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXTexManager.h"
#include"CameraMovementManager.h"
using namespace DirectX;
class SkyboxTestApplication :public DX12ApplicationManagerBase
{
public:
	SkyboxTestApplication();
	~SkyboxTestApplication();
	void PreRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	DXFPSCameraController m_maincameracontroller;
	DXTexture m_skyboxtex;
	DX12PSO m_pso,m_overlaypso;
	DX12RootSignature m_rootsignature,m_overlayrootsignature;
	Model m_planemodel;
	Model m_cubemodel;
	void InitPSO();
	void InitOverlayPSO();
	DX12DESCHEAP m_resourceaccessheap;
};