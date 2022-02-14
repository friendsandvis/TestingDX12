#pragma once
#include"DXUtils.h"
#include"DX12Resource.h"

struct samplerFeedbackTexInitData
{
	D3D12_RESOURCE_DESC1 resdesc;
	D3D12_HEAP_PROPERTIES heapprops;
	D3D12_HEAP_FLAGS heapflags;

};

class SamplerFeedbackTexture :public DX12Resource
{
public:


	void Init(ComPtr<ID3D12Device8> creationdevice, samplerFeedbackTexInitData initdata);

	//1 Sampler Feedbackmap can pair with 1 res only
	void Pair(ComPtr<ID3D12Device8> creationdevice, DX12Resource* restopairwith,D3D12_CPU_DESCRIPTOR_HANDLE uavhandle);

private:
	D3D12_RESOURCE_DESC1 m_resdesc;
	DX12Resource* m_pairedres;
};

struct DX12FeedBackUnit
{
public:
	DX12FeedBackUnit();
	~DX12FeedBackUnit();

private:
	
	SamplerFeedbackTexture m_feedbacktex;
};

