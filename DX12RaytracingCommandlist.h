#pragma once
#include"DXUtils.h"

class DX12RaytracingCommandlist
{
public:
	DX12RaytracingCommandlist();
	~DX12RaytracingCommandlist();
	void SetName(wstring name);
	void Init(D3D12_COMMAND_LIST_TYPE type,ComPtr< ID3D12Device> creationdevice, UINT numAllocators = 1);
	void Reset(bool closeafterreset=false, bool resetallocator=true, UINT allocatorIndexToUse = 0);
	inline ID3D12GraphicsCommandList* GetcmdList() { return m_list.Get(); }
	ComPtr<ID3D12GraphicsCommandList> GetcmdListComPtr() { return m_list; }
	ID3D12GraphicsCommandList4* operator->() { return m_list.Get(); }
	bool IsClose() { return m_isclosed; }
	void Close();
	wstring GetName() { return m_name; }

private:
	//currently bundling allocator and list together for simplicity but ideally should be seperate(given that an allocator can be used by multiple lists(but not more than 1 at the same time)
	ComPtr<ID3D12GraphicsCommandList4> m_list;
	bool m_isclosed;
	//index in allocators which is being used by commandlist(after resting the list)
	UINT m_currentlyUsedAllocatoridx;
	//we have allocators to support frames in flight(need to seperate allocator most used in framesinflight
	ComPtr<ID3D12CommandAllocator>* m_allocators;
	UINT m_numAllocators;
	wstring m_name;


};


