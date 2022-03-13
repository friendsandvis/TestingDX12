#pragma once
#include"DXUtils.h"
#include"DX12Resource.h"
#include"DX12Buffer.h"
#include"DX12Texture.h"
#include"DX12ReservedResourceMemoryManager.h"
#include"CommonDataStructures.h"






struct samplerFeedbackUnitInitData
{
	bool isMinMipMap = true;
	DX12ReservedResource* feedbacktexrestopairwith;
	D3D12_CPU_DESCRIPTOR_HANDLE feedbacktexuavhandle;
	D3D12_MIP_REGION feedbackmipregion;


};
struct SamplerFeedbackTextureInitData
{

	D3D12_RESOURCE_DESC1 feedbacktexdesc;
	D3D12_HEAP_PROPERTIES feedbacktexheapprops;
	D3D12_HEAP_FLAGS feedbacktexheapflags;
};

class SamplerFeedbackTexture :public DX12ResourceBase
{
public:
	SamplerFeedbackTexture();
	~SamplerFeedbackTexture();
	UINT64 GetRequiredBufferSizeForTranscodeing();
	void Init(ComPtr<ID3D12Device8> creationdevice, SamplerFeedbackTextureInitData initdata);
	void Readback(ComPtr<ID3D12GraphicsCommandList1> commandlist,DX12ResourceBase* dstres);

	//1 Sampler Feedbackmap can pair with 1 res only
	void Pair(ComPtr<ID3D12Device8> creationdevice, DX12ResourceBase* restopairwith,D3D12_CPU_DESCRIPTOR_HANDLE uavhandle);

private:
	D3D12_RESOURCE_DESC1 m_resdesc;
	DX12ResourceBase* m_pairedres;
};

//the feedback unit is exclusively ment for sfs implementation.
class DX12FeedBackUnit
{
public:
	DX12FeedBackUnit();
	~DX12FeedBackUnit();
	void InitReedbackBuffer();
	void Init(ComPtr<ID3D12Device8> creationdevice, samplerFeedbackUnitInitData initdata);
	//issue a command on the commandlist to resolve the feedbacktexture into the readable buffer.
	void Readback(ComPtr<ID3D12GraphicsCommandList1> commandlist);
	//NOTE::to be called only after the commandlist with resolve command has completed execution.This function processes the feedback data to perform desiered action on the target texture.
	void ProcessReadbackdata();

private:
	SamplerFeedbackTexture m_feedbacktex;
	DX12Buffer m_feedbackreadbackbuffer;
	DX12TextureSimple m_minlod;
	DX12ReservedResourceMemoryManager m_reservedresmemorymanager;
	//used to map unmap mips by comparing readback data
	Set<uint8_t> m_currentlymappedmips;
	
	
	
};

