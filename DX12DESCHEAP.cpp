#include"DX12DESCHEAP.h"


DX12DESCHEAP::DX12DESCHEAP()
	:m_handleincsize(0)
{
}

DX12DESCHEAP::~DX12DESCHEAP()
{
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12DESCHEAP::GetCPUHandlefromstart()
{
	
	
	
		return m_descheap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12DESCHEAP::GetGPUHandlefromstart()
{



	return m_descheap->GetGPUDescriptorHandleForHeapStart();
}


D3D12_CPU_DESCRIPTOR_HANDLE DX12DESCHEAP::GetCPUHandleOffseted(unsigned offetcountfromstart)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle=m_descheap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr = handle.ptr + ((SIZE_T)offetcountfromstart * (SIZE_T)m_handleincsize);

	return handle;
}

void DX12DESCHEAP::Init(D3D12_DESCRIPTOR_HEAP_DESC desc, ComPtr< ID3D12Device> creationdevice)
{
	m_heapdesc = desc;
	m_handleincsize=creationdevice->GetDescriptorHandleIncrementSize(desc.Type);

	DXASSERT(creationdevice->CreateDescriptorHeap(&m_heapdesc, IID_PPV_ARGS(m_descheap.GetAddressOf())))

	
}

