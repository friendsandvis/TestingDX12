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
#include"DX12RaytracingCommandlist.h"

using namespace DirectX;

class RayTracingApplication2 :public DX12ApplicationManagerBase
{
public:
	RayTracingApplication2();
	~RayTracingApplication2();
	void PreRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	struct BasicShaderRecord
	{
		uint8_t m_shaderidentifier[D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES];
		void SetShaderidentifier(void* identifier)
		{
			memcpy(m_shaderidentifier, identifier, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		}
	};
	DX12DESCHEAP m_gbufferrtvheaps;
	DX12DESCHEAP m_rtresheap_global, m_rtresheap_globalupload;
	DX12DESCHEAP m_rtdisplayresheap;
	DX12TextureSimple m_gbuffernormal,m_gbufferposition,m_rtouput;
	DX12PSO m_pso;
	//renderes the rt output texture as a full screen quad.
	DX12PSO m_psortdisplay;
	RTPSO m_simplertpso;
	//shader records
	DX12Buffer m_rgsrecords,m_missrecords,m_hitrecords;
	DX12RootSignature m_rootsignature;
	//device5 is often used in raytracing so retrive it once and keep troughout app lifetime.
	DX12RaytracingCommandlist m_rtcommandlist;
	ComPtr<ID3D12Device5> m_device5;
	Model m_loadedmodel,m_trianglemodel,m_planemodel;
	bool m_raytracingsupported;
	DXFPSCameraController m_maincameracontroller;
	ModelAccelerationStructureBLAS loadedmodelasblas;
	ModelAccelerationStructureTLAS loadedmodelastlas;
	void InitPSO();
	void InitPSO_RTRaster();
	void InitRTDisplayPSO();
	void InitRTPSO();
	void RenderRT();
	void RenderRaster();
};