#include"SamplerFeedbackManager.h"


DX12FeedBackUnit::DX12FeedBackUnit()
	
{
}

DX12FeedBackUnit::~DX12FeedBackUnit()
{
}


void SamplerFeedbackTexture::Init(ComPtr<ID3D12Device8> creationdevice, samplerFeedbackTexInitData initdata)
{
	m_resdesc = initdata.resdesc;
	DXASSERT(creationdevice->CreateCommittedResource2(&initdata.heapprops, initdata.heapflags, &initdata.resdesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())))
		m_currentresstate = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	m_rescreationmode = ResourceCreationMode::COMMITED;
	m_pairedres = nullptr;


}

void SamplerFeedbackTexture::Pair(ComPtr<ID3D12Device8> creationdevice, DX12Resource* restopairwith, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	//1 paired res only
	assert(m_pairedres == nullptr);
	m_pairedres = restopairwith;

	//create UAV as well
	creationdevice->CreateSamplerFeedbackUnorderedAccessView(m_pairedres->GetResource().Get(), m_resource.Get(), uavhandle);

}