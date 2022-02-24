#include"DX12Resource.h"

D3D12_RESOURCE_BARRIER DX12ResourceBase::TransitionResState(D3D12_RESOURCE_STATES targetstate)
{
	D3D12_RESOURCE_BARRIER transitionbarrier = {};

	//return a default barrier struct if target state is same as current state
	if (targetstate != m_currentresstate)
	{
		transitionbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		transitionbarrier.Transition.StateBefore = m_currentresstate;
		transitionbarrier.Transition.StateAfter = targetstate;
		transitionbarrier.Transition.pResource = m_resource.Get();
		m_currentresstate = targetstate;
	}
	return transitionbarrier;
}

void DX12Resource::CreateUAV(ComPtr< ID3D12Device> creationdevice, D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	creationdevice->CreateUnorderedAccessView(m_resource.Get(), nullptr, &uavdesc, uavhandle);
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

void DX12Resource::InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops)
{
	rescreationprops.useclearvalue = false;
	rescreationprops.resinitialstate = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	rescreationprops.resheapprop.Type = D3D12_HEAP_TYPE_DEFAULT;
	rescreationprops.resheapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	rescreationprops.resheapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	rescreationprops.resheapflags = D3D12_HEAP_FLAG_NONE;
	rescreationprops.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rescreationprops.resdesc.Alignment = 0;
	rescreationprops.resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	rescreationprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rescreationprops.resdesc.DepthOrArraySize = 1;
	rescreationprops.resdesc.MipLevels = 1;
	rescreationprops.resdesc.SampleDesc.Count = 1;
	rescreationprops.resdesc.SampleDesc.Quality = 0;
	rescreationprops.resdesc.Height = 128;
	rescreationprops.resdesc.Width = 128;
	rescreationprops.resdesc.Format = DXGI_FORMAT_UNKNOWN;
	rescreationprops.resheapprop.CreationNodeMask = 0;
	rescreationprops.resheapprop.VisibleNodeMask = 0;
}



void DX12ReservedResource::Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops)
{
	m_creationproperties = resprops;
	m_rescreationmode = ResourceCreationMode::RESERVED;
	SetResState(m_creationproperties.resinitialstate);

	D3D12_CLEAR_VALUE* clearvalue = nullptr;
	if (m_creationproperties.useclearvalue)
	{
		clearvalue = &m_creationproperties.optimizedclearvalue;
	}

	UINT numtilesforfullresource;
	//NOTE:using mipcounts as total subresources in the reserved res(might need to change if needed)
	UINT numsubresourcetiling = m_creationproperties.resdesc.MipLevels;
	m_subresourcetiling.resize(numsubresourcetiling);
	DXASSERT(creationdevice->CreateReservedResource(&m_creationproperties.resdesc, m_creationproperties.resinitialstate, clearvalue, IID_PPV_ARGS(m_resource.GetAddressOf())))
		//retrive resource tiling info as well
		creationdevice->GetResourceTiling(m_resource.Get(), &numtilesforfullresource, &m_packedmipinfo, nullptr, &numsubresourcetiling,0,m_subresourcetiling.data());

}
void DX12ReservedResource::CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle)
{
	creationdevice->CreateShaderResourceView(m_resource.Get(), &srvdesc, srvhandle);
}
void DX12ReservedResource::CreateUAV(ComPtr< ID3D12Device> creationdevice, D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	creationdevice->CreateUnorderedAccessView(m_resource.Get(), nullptr, &uavdesc, uavhandle);
}
DX12ReservedResource::DX12ReservedResource()
{}
DX12ReservedResource::~DX12ReservedResource()
{}

void DX12ReservedResource::InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops)
{
	//same as any generic resource
	DX12Resource::InitResourceCreationProperties(rescreationprops);
	//some special properties for reserved resource
	rescreationprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
}



DX12ReservedresourcePhysicalMemoryManager::DX12ReservedresourcePhysicalMemoryManager()
{}

DX12ReservedresourcePhysicalMemoryManager::~DX12ReservedresourcePhysicalMemoryManager()
{}