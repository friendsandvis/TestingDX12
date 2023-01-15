#include"DX12ReservedResource.h"
#include"DX12Resource.h"
#include"StreamableTextureFileReader.h"

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

	
	
	DXASSERT(creationdevice->CreateReservedResource(&m_creationproperties.resdesc, m_creationproperties.resinitialstate, clearvalue, IID_PPV_ARGS(m_resource.GetAddressOf())))
		InitTilingInfo(creationdevice);

}

void DX12ReservedResource::Init(ComPtr< ID3D12Device> creationdevice, bool forceallowunorderedaccess)
{
	m_rescreationmode = ResourceCreationMode::RESERVED;
	SetResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
	//prepare resource creation props from dx data & create resource from it.
	InitResourceCreationProperties(m_creationproperties);
	m_creationproperties.resdesc.Format = m_imagedata.m_imagemetadata.format;
	m_creationproperties.resdesc.Width = m_imagedata.m_imagemetadata.width;
	m_creationproperties.resdesc.Height = m_imagedata.m_imagemetadata.height;
	m_creationproperties.resdesc.MipLevels = m_imagedata.m_imagemetadata.mipLevels;
	if (forceallowunorderedaccess)
	{
		m_creationproperties.resdesc.Flags |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	DXASSERT((creationdevice->CreateReservedResource(&m_creationproperties.resdesc, m_currentresstate, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf()))))
		InitTilingInfo(creationdevice);

}

void DX12ReservedResource::InitTilingInfo(ComPtr< ID3D12Device> creationdevice)
{
	UINT numtilesforfullresource;
	//NOTE:using mipcounts as total subresources in the reserved res(might need to change if needed)
	UINT numsubresourcetiling = m_creationproperties.resdesc.MipLevels;
	m_subresourcetiling.resize(numsubresourcetiling);
	creationdevice->GetResourceTiling(m_resource.Get(), &numtilesforfullresource, &m_packedmipinfo, nullptr, &numsubresourcetiling, 0, m_subresourcetiling.data());
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
	:m_stfreader(nullptr)
{}
DX12ReservedResource::~DX12ReservedResource()
{
}

void DX12ReservedResource::InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops)
{
	//same as any generic resource
	DX12Resource::InitResourceCreationProperties(rescreationprops);
	//some special properties for reserved resource
	rescreationprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
}
void DX12ReservedResource::InitStreamable(ComPtr< ID3D12Device> creationdevice, shared_ptr<StreamableTextureFileReader> stfreadertoinitwith, bool forceallowunorderedaccess)
{
	m_stfreader = stfreadertoinitwith;
	m_rescreationmode = ResourceCreationMode::RESERVED;
	SetResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
	//prepare resource creation props from dx data & create resource from it.
	InitResourceCreationProperties(m_creationproperties);
	TexMetadata metadata=m_stfreader->GetMetaData();
	m_creationproperties.resdesc.Format = metadata.format;
	m_creationproperties.resdesc.Width = metadata.width;
	m_creationproperties.resdesc.Height = metadata.height;
	m_creationproperties.resdesc.MipLevels = metadata.mipLevels;
	if (forceallowunorderedaccess)
	{
		m_creationproperties.resdesc.Flags |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	
	DXASSERT((creationdevice->CreateReservedResource(&m_creationproperties.resdesc, m_currentresstate, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf()))))
		InitTilingInfo(creationdevice);


}