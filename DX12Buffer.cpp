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

void DX12Buffer::Init(const ComPtr< ID3D12Device> creationdevice, BufferCreationProperties bufferresprop, ResourceCreationMode creationmode)
{
	m_properties = bufferresprop;
	m_rescreationmode = creationmode;
	m_is_uploadtype = (bufferresprop.resheapprop.Type== D3D12_HEAP_TYPE_UPLOAD);
	

	CreateBuffer(creationdevice);
	
}

void DX12Buffer::CreateBuffer(ComPtr< ID3D12Device> creationdevice)
{

	switch (m_rescreationmode)
	{

	case COMMITED:
	{
		DXASSERT(creationdevice->CreateCommittedResource(&m_properties.resheapprop, m_properties.resheapflags, &m_properties.resdesc, m_properties.resinitialstate, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())))
			m_resource->SetName(L"Buffer");
		SetResState(m_properties.resinitialstate);
		break;
	}

	//to implement other cration modes

	default:
		break;
	}
}