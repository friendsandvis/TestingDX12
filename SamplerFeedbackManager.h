#pragma once
#include"DXUtils.h"
#include"DX12Resource.h"
#include"DX12Buffer.h"

struct samplerFeedbackTexInitData
{
	D3D12_RESOURCE_DESC1 resdesc;
	D3D12_HEAP_PROPERTIES heapprops;
	D3D12_HEAP_FLAGS heapflags;

};

class SamplerFeedbackTexture :public DX12ResourceBase
{
public:
	SamplerFeedbackTexture();
	~SamplerFeedbackTexture();
	UINT64 GetRequiredBufferSizeForTranscodeing();
	void Init(ComPtr<ID3D12Device8> creationdevice, samplerFeedbackTexInitData initdata);
	void Readback(ComPtr<ID3D12GraphicsCommandList1> commandlist,DX12ResourceBase* dstres);

	//1 Sampler Feedbackmap can pair with 1 res only
	void Pair(ComPtr<ID3D12Device8> creationdevice, DX12ResourceBase* restopairwith,D3D12_CPU_DESCRIPTOR_HANDLE uavhandle);

private:
	D3D12_RESOURCE_DESC1 m_resdesc;
	DX12ResourceBase* m_pairedres;
};

struct DX12FeedBackUnit
{
	DX12FeedBackUnit();
	~DX12FeedBackUnit();
	void InitReedbackBuffer();


	
	SamplerFeedbackTexture m_feedbacktex;
	DX12Buffer m_feedbackreadbackbuffer;
};

