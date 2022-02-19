#pragma once
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"AssetManager.h"
#include"DXTexManager.h"


class DX12ComputeApplication:public DX12ApplicationManagerBase
{
public:
	DX12ComputeApplication();
	~DX12ComputeApplication();
	void Render()override;

protected:
	void InitExtras() override;

private:
	DX12PSO m_gfxpso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob, m_rootsignatureerrors;
	ComPtr<ID3D12RootSignature> m_emptyrootsignature;
	Model m_planemodel;
	DXTexture m_redtexture;
	DX12DESCHEAP m_resaccessviewdescheap;//CBV/SRV/UAV
	void InitGfxPSO();

};

