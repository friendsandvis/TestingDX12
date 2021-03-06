#include"DX12Resource.h"




void DX12Resource::CreateUAV(ComPtr< ID3D12Device> creationdevice, D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	creationdevice->CreateUnorderedAccessView(m_resource.Get(), nullptr, &uavdesc, uavhandle);
}

void DX12Resource::CreateSRV(ComPtr< ID3D12Device> creationdevice,D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc,D3D12_CPU_DESCRIPTOR_HANDLE srvhandle)
{
	creationdevice->CreateShaderResourceView(m_resource.Get(), &srvdesc, srvhandle);
	
}
void DX12Resource::CreateDSV(ComPtr< ID3D12Device> creationdevice, D3D12_DEPTH_STENCIL_VIEW_DESC dsvdesc, D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle)
{
	creationdevice->CreateDepthStencilView(m_resource.Get(),&dsvdesc,dsvhandle);
}
void DX12Resource::CreateRTV(ComPtr< ID3D12Device> creationdevice, D3D12_RENDER_TARGET_VIEW_DESC rtvdesc, D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle)
{
	creationdevice->CreateRenderTargetView(m_resource.Get(), &rtvdesc, rtvhandle);
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
	rescreationprops.resdesc.Height = 1;
	rescreationprops.resdesc.Width = 1;
	rescreationprops.resdesc.Format = DXGI_FORMAT_UNKNOWN;
	rescreationprops.resheapprop.CreationNodeMask = 0;
	rescreationprops.resheapprop.VisibleNodeMask = 0;
}







				
			
		
