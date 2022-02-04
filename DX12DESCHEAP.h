#pragma once
#include"DXUtils.h"

class DX12DESCHEAP
{
public:
	DX12DESCHEAP();
	~DX12DESCHEAP();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandlefromstart();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandlefromstart();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleOffseted(unsigned offetcountfromstart);

	void Init(D3D12_DESCRIPTOR_HEAP_DESC desc, ComPtr< ID3D12Device> creationdevice);
	inline ID3D12DescriptorHeap* GetDescHeap() { return m_descheap.Get(); }
	
private:
	ComPtr<ID3D12DescriptorHeap> m_descheap;
	D3D12_DESCRIPTOR_HEAP_DESC m_heapdesc;
	UINT m_handleincsize;


};

