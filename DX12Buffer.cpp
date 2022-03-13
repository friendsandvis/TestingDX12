#include"DX12Buffer.h"


void* DX12Buffer::Map(BufferMapParams readparams)
{
	void* mappedbuffer=nullptr;
	DXASSERT(m_resource->Map(readparams.subresource,&readparams.range,&mappedbuffer))
	return mappedbuffer;
}

void DX12Buffer::UnMap(BufferMapParams writeparams)
{
	m_resource->Unmap(writeparams.subresource, &writeparams.range);
}

void DX12Buffer::InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops)
{
	rescreationprops.useclearvalue = false;
	rescreationprops.resinitialstate = D3D12_RESOURCE_STATE_COMMON;
	rescreationprops.resheapprop.Type = D3D12_HEAP_TYPE_DEFAULT;
	rescreationprops.resheapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	rescreationprops.resheapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	rescreationprops.resheapflags = D3D12_HEAP_FLAG_NONE;
	rescreationprops.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rescreationprops.resdesc.Alignment = 0;
	rescreationprops.resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	rescreationprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
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
