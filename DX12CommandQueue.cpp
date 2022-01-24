#include"DX12CommandQueue.h"

DX12CommandQueue::DX12CommandQueue()
{
}

DX12CommandQueue::~DX12CommandQueue()
{
}

void DX12CommandQueue::Init(D3D12_COMMAND_QUEUE_DESC queuedesc, ComPtr<ID3D12Device> creationdevice)
{
	m_desc = queuedesc;

	DXASSERT(creationdevice->CreateCommandQueue(&m_desc,IID_PPV_ARGS(m_queue.GetAddressOf())))
}