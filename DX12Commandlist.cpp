#include"DX12CommandList.h"

void DX12Commandlist::Init(D3D12_COMMAND_LIST_TYPE type,ComPtr< ID3D12Device> creationdevice, UINT numAllocators)
{
	m_numAllocators = numAllocators;
	assert(m_allocators == nullptr);
	m_allocators = new ComPtr<ID3D12CommandAllocator>[m_numAllocators]();
	//allocator
	for (UINT i = 0; i < m_numAllocators; i++)
	{
		ComPtr<ID3D12CommandAllocator>& allocatorToInit = m_allocators[i];
		DXASSERT(creationdevice->CreateCommandAllocator(type, IID_PPV_ARGS(allocatorToInit.GetAddressOf())))
	}
		//list
		DXASSERT(creationdevice->CreateCommandList(0, type, m_allocators[m_currentlyUsedAllocatoridx].Get(), nullptr, IID_PPV_ARGS(m_list.GetAddressOf())))
		DXASSERT(m_list->Close())
		m_isclosed = true;
}

void DX12Commandlist::SetName(wstring name)
{
	wstring listname = name + L"List";
	wstring allocatorname = name + L"Allocator";
	DXASSERT(m_list->SetName(listname.c_str()))
		m_name = listname;
	for (int i = 0; i < m_numAllocators; i++)
	{
		wstring allocatorname = name + L"Allocator_0";

		if (i != 0)
		{
			wstring Idxsuffix;
			DXUtils::GetIdxSuffix(i, Idxsuffix);
			allocatorname = (name + L"Allocator_");
			allocatorname = allocatorname + Idxsuffix;
		}
		DXASSERT(m_allocators[i]->SetName(allocatorname.c_str()))
	}
}

void DX12Commandlist::Reset(bool closeafterreset,bool resetallocator, UINT allocatorIndexToUse)
{
	if (resetallocator)
	{
		
		DXASSERT(m_allocators[allocatorIndexToUse]->Reset());
	}
	DXASSERT(m_list->Reset(m_allocators[allocatorIndexToUse].Get(), nullptr))
		m_isclosed = false;
	m_currentlyUsedAllocatoridx = allocatorIndexToUse;

		if (closeafterreset)
		{
			
			Close();
	}
}
DX12Commandlist::DX12Commandlist()
	:m_isclosed(true),
	m_name(L"unknown"),
	m_numAllocators(0),
	m_currentlyUsedAllocatoridx(0),
	m_allocators(nullptr)
{}
DX12Commandlist::~DX12Commandlist()
{
	if (m_allocators)
	{
		delete[] m_allocators;
	}
}

void DX12Commandlist::Close()
{
	//if (!m_isclosed)
	{
		DXASSERT(m_list->Close())
			m_isclosed = true;
	}
}
