#include"DX12Buffer.h"


void DX12BUFFER::Init(const ComPtr< ID3D12Device> creationdevice, BufferCreationProperties bufferresprop, ResourceCreationMode creationmode)
{
	m_properties = bufferresprop;
	m_rescreationmode = creationmode;
	

	CreateBuffer(creationdevice);
	
}

void DX12BUFFER::CreateBuffer(ComPtr< ID3D12Device> creationdevice)
{

	switch (m_rescreationmode)
	{

	case COMMITED:
	{
		DXASSERT(creationdevice->CreateCommittedResource(&m_properties.resheapprop, m_properties.resheapflags, &m_properties.resdesc,m_properties.resinitialstate, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())))
		break;
	}

	//to implement other cration modes

	default:
		break;
	}
}