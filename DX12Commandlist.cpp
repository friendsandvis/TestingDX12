#include"DX12CommandList.h"

void DX12Commandlist::Init(D3D12_COMMAND_LIST_TYPE type,ComPtr< ID3D12Device> creationdevice)
{
	//allocator
	DXASSERT(creationdevice->CreateCommandAllocator(type, IID_PPV_ARGS(m_allocator.GetAddressOf())))
		//list
		DXASSERT(creationdevice->CreateCommandList(0, type, m_allocator.Get(), nullptr, IID_PPV_ARGS(m_list.GetAddressOf())))
		DXASSERT(m_list->Close())
}

void DX12Commandlist::SetName(LPCWSTR name)
{
	DXASSERT(m_list->SetName(name))
}

void DX12Commandlist::Reset(bool closeafterreset)
{
	DXASSERT(m_list->Reset(m_allocator.Get(),nullptr))

		if (closeafterreset)
		{
			DXASSERT(m_list->Close())
	}
}


DX12Commandlist::DX12Commandlist()
{
}

DX12Commandlist::~DX12Commandlist()
{
}
