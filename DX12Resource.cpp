#include"DX12Resource.h"

D3D12_RESOURCE_BARRIER DX12ResourceBase::TransitionResState(D3D12_RESOURCE_STATES targetstate)
{
	D3D12_RESOURCE_BARRIER transitionbarrier = {};
	transitionbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	transitionbarrier.Transition.StateBefore = m_currentresstate;
	transitionbarrier.Transition.StateAfter = targetstate;
	transitionbarrier.Transition.pResource = m_resource.Get();
	m_currentresstate = targetstate;

	return transitionbarrier;
}

void DX12Resource::CreateSRV(ComPtr< ID3D12Device> creationdevice,D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc,D3D12_CPU_DESCRIPTOR_HANDLE srvhandle)
{
	creationdevice->CreateShaderResourceView(m_resource.Get(), &srvdesc, srvhandle);
	
}

void DX12Resource::Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops, ResourceCreationMode creationmode)
{
	m_creationproperties = resprops;
	m_rescreationmode = creationmode;
	SetResState(m_creationproperties.resinitialstate);
	D3D12_CLEAR_VALUE* optimizedclearvalue = nullptr; &m_creationproperties.optimizedclearvalue;
	if (m_creationproperties.useclearvalue)
		optimizedclearvalue = &m_creationproperties.optimizedclearvalue;

	switch (m_rescreationmode)
	{
	case ResourceCreationMode::COMMITED:
		DXASSERT(creationdevice->CreateCommittedResource(&m_creationproperties.resheapprop,m_creationproperties.resheapflags,&m_creationproperties.resdesc,m_creationproperties.resinitialstate,optimizedclearvalue,IID_PPV_ARGS(m_resource.GetAddressOf())))
		break;

	default:
		break;
	}
}
