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
#define NUMGBUFFERTEXTURES 3
using namespace DirectX;

class RayTracingApplicationAdvanced :public DX12ApplicationManagerBase
{
public:
	RayTracingApplicationAdvanced();
	~RayTracingApplicationAdvanced();
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
	
	DX12DESCHEAP m_gbufferrtvheaps,m_gbuffersrvheap;
	DX12DESCHEAP m_rtresheap_global, m_rtresheap_globalupload;
	DX12DESCHEAP m_rtdisplayresheap;
	DX12TextureSimple m_gbuffernormal,m_gbufferposition, m_gbufferalbedo,m_rtouput;
	DX12PSO m_pso,m_gbufferpso;
	//renderes the rt output texture as a full screen quad.
	DX12PSO m_psortdisplay,m_psogbufferdisplay;
	RTPSO m_simplertpso;
	//shader records
	DX12Buffer m_rgsrecords,m_missrecords,m_hitrecords;
	
	//redender in raytracing mode otherwise in rasterization 
	bool m_rtmode;
	//device5 is often used in raytracing so retrive it once and keep troughout app lifetime.
	DX12RaytracingCommandlist m_rtcommandlist;
	DX12Commandlist m_gbufferrendercommandlist;
	ComPtr<ID3D12Device5> m_device5;
	Model m_loadedmodel,m_trianglemodel,m_planemodel;
	CompoundModel m_comloadedmodel;
	bool m_raytracingsupported;
	DXFPSCameraController m_maincameracontroller;
	ModelAccelerationStructureBLAS loadedmodelasblas;
	CompoundModelAccelerationStructureBLAS m_comploadedmodelblas;
	ModelAccelerationStructureTLAS loadedmodelastlas;
	void InitGbufferPSO();
	void InitPSO_RTRaster();
	void InitRTDisplayPSO();
	void InitGBufferDisplayPSO();
	void InitRTPSO();
	void RenderRT();
	void RenderRaster();
	void RenderGbuffer();
	void RenderTextureOnScreenRT();
	void RenderTextureOnScreenGBuffer();
};