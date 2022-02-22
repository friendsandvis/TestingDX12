#include"SamplerFeedbackManager.h"


DX12FeedBackUnit::DX12FeedBackUnit()
{
}

DX12FeedBackUnit::~DX12FeedBackUnit()
{
}

void DX12FeedBackUnit::InitReedbackBuffer()
{
	DX12ResourceCreationProperties readbackbufferprops;
	DX12Resource::InitResourceCreationProperties(readbackbufferprops);
	readbackbufferprops.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	readbackbufferprops.resdesc.Width = m_feedbacktex.GetRequiredBufferSizeForTranscodeing();
	readbackbufferprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	readbackbufferprops.resheapprop.Type = D3D12_HEAP_TYPE_READBACK;
}


SamplerFeedbackTexture::SamplerFeedbackTexture()
	:m_pairedres(nullptr)
{}
SamplerFeedbackTexture::~SamplerFeedbackTexture()
{}

UINT64 SamplerFeedbackTexture::GetRequiredBufferSizeForTranscodeing()
{
	//make sure resource is paired 
	assert(m_pairedres!=nullptr);
	UINT64 buffersize = (UINT64)ceil(m_resdesc.Height / (double)m_resdesc.SamplerFeedbackMipRegion.Height);
	buffersize *= ((UINT64)ceil(m_resdesc.Width / (double)m_resdesc.SamplerFeedbackMipRegion.Width));
	return buffersize;

}

void SamplerFeedbackTexture::Readback(ComPtr<ID3D12GraphicsCommandList1> commandlist, DX12ResourceBase* dstres)
{
	D3D12_RESOURCE_BARRIER barriers[2];
	ComPtr<ID3D12Resource> dstresource = dstres->GetResource();
	barriers[0]=dstres->TransitionResState(D3D12_RESOURCE_STATE_RESOLVE_DEST);
	barriers[1] = TransitionResState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
	commandlist->ResourceBarrier(2, barriers);


	commandlist->ResolveSubresourceRegion(dstresource.Get(), 0, 0, 0, m_resource.Get(), 0, nullptr, DXGI_FORMAT_R8_UINT, D3D12_RESOLVE_MODE_DECODE_SAMPLER_FEEDBACK);
	//transition feedback res back to uav after resolve
	barriers[1] = TransitionResState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandlist->ResourceBarrier(1, &barriers[1]);
}

void SamplerFeedbackTexture::Init(ComPtr<ID3D12Device8> creationdevice, samplerFeedbackTexInitData initdata)
{
	m_resdesc = initdata.resdesc;
	DXASSERT(creationdevice->CreateCommittedResource2(&initdata.heapprops, initdata.heapflags, &initdata.resdesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())))
		m_currentresstate = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	m_rescreationmode = ResourceCreationMode::COMMITED;
	m_pairedres = nullptr;


}

void SamplerFeedbackTexture::Pair(ComPtr<ID3D12Device8> creationdevice, DX12ResourceBase* restopairwith, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	//1 paired res only
	assert(m_pairedres == nullptr);
	m_pairedres = restopairwith;

	//create UAV as well
	creationdevice->CreateSamplerFeedbackUnorderedAccessView(m_pairedres->GetResource().Get(), m_resource.Get(), uavhandle);

}