#pragma once
#include"DXUtils.h"
#include"DX12Resource.h"
#include"DX12Buffer.h"
#include"DX12Texture.h"
#include"DX12ReservedResourceMemoryManager.h"
#include"CommonDataStructures.h"
#include"DX12DESCHEAP.h"

/*
In the current implementation we are not usin the residency map for making the implementation a bit simple Given that we know the whole texture will be visible at the same time so we can simple get away will a lod clamp constant sent to shader.
*/




struct samplerFeedbackUnitInitData
{
	bool allowreservedresourceuavclear = false;
	bool isMinMipMap = true;
	DX12ReservedResource* feedbacktexrestopairwith;
	D3D12_CPU_DESCRIPTOR_HANDLE feedbacktexuavhandle;
	D3D12_CPU_DESCRIPTOR_HANDLE minlodtexsrvhandle;
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
	const unsigned GetLODClampValue() { return m_lodclampvalue; }
	void Init(ComPtr<ID3D12Device8> creationdevice, samplerFeedbackUnitInitData initdata);
	//issue a command on the commandlist to resolve the feedbacktexture into the readable buffer.
	void Readback(ComPtr<ID3D12GraphicsCommandList1> commandlist);
	//NOTE::to be called only after the commandlist with resolve command has completed execution.This function processes the feedback data to perform desiered action on the target texture.
	void ProcessReadbackdata();
	//if any mip level is needed to be bound out of the normal streaming cycle
	void BindMipLevel(uint8_t mipleveltobind, bool makeunmapable = false);
	void ClearReservedResourceMip(DX12Commandlist& cmdlist, uint8_t mipindextoclear, float* clearcolour);
	SamplerFeedbackTexture& GetFeedbackTexture() { return m_feedbacktex; }
	//updates the physical memory mapping for the reserved resource(queue needed for updating the mappings and device for creating new heaps if needed)
	void UpdateMemoryMappings(ComPtr<ID3D12CommandQueue>commandqueue, ComPtr< ID3D12Device> creationdevice)
	{
		m_reservedresmemorymanager.Update(commandqueue, creationdevice);
	}
	//upload any data to managed reserved resource texture if needed
	void UploadTextureData(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& uploadcmdlist)
	{
		m_reservedresmemorymanager.UploadData(creationdevice, uploadcmdlist);
	}
private:
	void TryUpdateLODClamp(unsigned loadedlodidx);
	SamplerFeedbackTexture m_feedbacktex;
	DX12Buffer m_feedbackreadbackbuffer;
	
	
	DX12ReservedResourceMemoryManager m_reservedresmemorymanager;
	//used to map unmap mips by comparing readback data
	Set<uint8_t> m_currentlymappedmips;
	//heaps needed for being able to clear the resource colour if needed
	DX12DESCHEAP m_uavheapupload, m_uavheap;
	//lod clamp value is used to clamp the mip level access of the managed reserved resource texture to prevent it from accessing non resident mips.
	unsigned m_lodclampvalue=0;
	
	
	
};

