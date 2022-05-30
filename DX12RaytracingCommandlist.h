#pragma once
#include"DXUtils.h"

class DX12RaytracingCommandlist
{
public:
	DX12RaytracingCommandlist();
	~DX12RaytracingCommandlist();
	void SetName(wstring name);
	void Init(D3D12_COMMAND_LIST_TYPE type,ComPtr< ID3D12Device> creationdevice);
	void Reset(bool closeafterreset=false, bool resetallocator=true);
	inline ID3D12GraphicsCommandList* GetcmdList() { return m_list.Get(); }
	ComPtr<ID3D12GraphicsCommandList> GetcmdListComPtr() { return m_list; }
	ID3D12GraphicsCommandList4* operator->() { return m_list.Get(); }
	bool IsClose() { return m_isclosed; }
	void Close();

private:
	//currently bundling allocator and list together for simplicity but ideally should be seperate(given that an allocator can be used by multiple lists(but not more than 1 at the same time)

	ComPtr<ID3D12CommandAllocator> m_allocator;
	ComPtr<ID3D12GraphicsCommandList4> m_list;
	bool m_isclosed;


};


