#include"DX12CommandList.h"

void DX12Commandlist::Init(D3D12_COMMAND_LIST_TYPE type,ComPtr< ID3D12Device> creationdevice)
{
	//allocator
	DXASSERT(creationdevice->CreateCommandAllocator(type, IID_PPV_ARGS(m_allocator.GetAddressOf())))
		//list
		DXASSERT(creationdevice->CreateCommandList(0, type, m_allocator.Get(), nullptr, IID_PPV_ARGS(m_list.GetAddressOf())))
		DXASSERT(m_list->Close())
		m_isclosed = true;
}

void DX12Commandlist::SetName(wstring name)
{
	wstring listname = name + L"List";
	wstring allocatorname = name + L"Allocator";
	DXASSERT(m_list->SetName(listname.c_str()))
		DXASSERT(m_allocator->SetName(allocatorname.c_str()))
}

void DX12Commandlist::Reset(bool closeafterreset,bool resetallocator)
{
	if (resetallocator)
	{
		
		DXASSERT(m_allocator->Reset());
	}
	DXASSERT(m_list->Reset(m_allocator.Get(), nullptr))
		m_isclosed = false;

		if (closeafterreset)
		{
			
			Close();
	}
}


DX12Commandlist::DX12Commandlist()
	:m_isclosed(true)
{
}

DX12Commandlist::~DX12Commandlist()
{
}

void DX12Commandlist::Close()
{
	//if (!m_isclosed)
	{
		DXASSERT(m_list->Close())
			m_isclosed = true;
	}
}
