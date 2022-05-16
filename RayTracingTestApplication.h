#pragma once
#include"DXUtils.h"
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
#include"CameraMovementManager.h"
#include"DX12DESCHEAP.h"
#include"DX12Texture.h"
#include"RaytracingUtils.h"

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
	DX12DESCHEAP m_gbufferrtvheaps;
	DX12TextureSimple m_gbuffernormal,m_gbufferposition;
	DX12PSO m_pso;
	DX12RootSignature m_rootsignature;
	
	Model m_loadedmodel;
	bool m_raytracingsupported;
	DXFPSCameraController m_maincameracontroller;
	ModelAccelerationStructureBLAS loadedmodelasblas;
	ModelAccelerationStructureTLAS loadedmodelastlas;
	void InitPSO();
};