#pragma once
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"AssetManager.h"
#include"DXTexManager.h"
#include"DX12Texture.h"


class DX12ComputeApplication:public DX12ApplicationManagerBase
{
public:
	DX12ComputeApplication();
	~DX12ComputeApplication();
	void Render()override;

protected:
	void InitExtras() override;

private:
	DX12PSO m_gfxpso,m_computepso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob, m_rootsignatureerrors,m_computerootsignatureblob,m_computerootsignatureerrors;
	ComPtr<ID3D12RootSignature> m_emptyrootsignature,m_computerootsignature;
	Model m_planemodel;
	DXTexture m_redtexture;
	DX12TextureSimple m_customtexture;
	
	DX12DESCHEAP m_resaccessviewdescheap;//CBV/SRV/UAV
	DX12DESCHEAP m_resaccessviewdescheap_compute;//CBV/SRV/UAV
	DX12DESCHEAP m_resaccessviewdescheap_computeupload;//CBV/SRV/UAV

	void InitGfxPSO();
	void InitComputePSO();

};

