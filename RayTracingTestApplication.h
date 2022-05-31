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
#include"RTPSO.h"

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
	DX12DESCHEAP m_rtresheap_global;
	DX12DESCHEAP m_rtdisplayresheap;
	DX12TextureSimple m_gbuffernormal,m_gbufferposition,m_rtouput;
	DX12PSO m_pso;
	//renderes the rt output texture as a full screen quad.
	DX12PSO m_psortdisplay;
	RTPSO m_simplertpso;
	DX12RootSignature m_rootsignature;
	//device5 is often used in raytracing so retrive it once and keep troughout app lifetime.
	DX12Commandlist m_rtcommandlist;
	ComPtr<ID3D12Device5> m_device5;
	Model m_loadedmodel,m_planemodel;
	bool m_raytracingsupported;
	DXFPSCameraController m_maincameracontroller;
	ModelAccelerationStructureBLAS loadedmodelasblas;
	ModelAccelerationStructureTLAS loadedmodelastlas;
	void InitPSO();
	void InitRTDisplayPSO();
	void InitRTPSO();
};