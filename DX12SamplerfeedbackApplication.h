#pragma once
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"AssetManager.h"
#include"DXTexManager.h"
#include"SamplerFeedbackManager.h"


class DX12SamplerfeedbackApplication :public DX12ApplicationManagerBase
{
public:
	DX12SamplerfeedbackApplication();
	~DX12SamplerfeedbackApplication();
	void Render()override;


protected:
	void InitExtras();

private:
	DX12PSO m_basicpso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob, m_rootsignatureerrors;
	ComPtr<ID3D12RootSignature> m_emptyrootsignature;
	Model m_planemodel;
	DXTexture m_redtexture;
	DX12DESCHEAP m_resaccessviewdescheap;//CBV/SRV/UAV
	DX12FeedBackUnit m_redtexfeedbackunit;
	

	void InitBasicPSO();

};


