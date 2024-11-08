#pragma once
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"AssetManager.h"
#include"DXTexManager.h"
#include"SamplerFeedbackManager.h"
#include"DXCamera.h"
#define USESTFTEXTURE

class DX12SamplerfeedbackApplication :public DX12ApplicationManagerBase
{
public:
	DX12SamplerfeedbackApplication();
	~DX12SamplerfeedbackApplication();
	void PostRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;


protected:
	void InitExtras()override;

private:
	DX12PSO m_basicpso,m_overlaypso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob, m_rootsignatureerrors;
	ComPtr<ID3D12RootSignature> m_emptyrootsignature;
	Model m_planemodel;
	DXTexture m_redtexture;
	DX12DESCHEAP m_resaccessviewdescheap,m_overlayresheap;//CBV/SRV/UAV
	DX12DESCHEAP m_resaccessviewdescheapsrc;//CBV/SRV/UAV(staging)
	DX12FeedBackUnit m_redtexfeedbackunit;
	DX12ReservedResource m_sfsreservedresourcetex;
	bool m_sfsupported;
	DX12RootSignature m_rootsignature;
	DX12Commandlist m_reservedresuploadcmdlist;
	
	

	void InitBasicPSO();
	void InitOverlayPSO();

};


