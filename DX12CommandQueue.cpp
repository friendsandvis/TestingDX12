#include"DX12CommandQueue.h"
#define USESPINLOCKFORFENCEWAIT 0

DX12CommandQueue::DX12CommandQueue()
	
{
}

DX12CommandQueue::~DX12CommandQueue()
{
}

void DX12CommandQueue::Init(D3D12_COMMAND_QUEUE_DESC queuedesc, ComPtr<ID3D12Device> creationdevice)
{
	m_desc = queuedesc;

	DXASSERT(creationdevice->CreateCommandQueue(&m_desc, IID_PPV_ARGS(m_queue.GetAddressOf())))
		m_queue->SetName(L"DXCMDQUEUE");
}


SyncronizationUnit::SyncronizationUnit()
	:m_currentvalue(0),
	m_expectedvalue(0),
	m_event(NULL)
{
}

SyncronizationUnit::~SyncronizationUnit()
{
}

void SyncronizationUnit::Init(ComPtr<ID3D12Device> creationdevice, UINT64 initvalue)
{
	m_currentvalue = initvalue;
	m_expectedvalue = initvalue;
	DXASSERT(creationdevice->CreateFence(m_currentvalue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())))
		m_event=CreateEvent(NULL,FALSE, FALSE, NULL);
	assert(m_event != NULL);
}

void SyncronizationUnit::SignalFence(ComPtr<ID3D12CommandQueue> queue, UINT64 signalvalue)
{
	m_currentvalue = m_expectedvalue = signalvalue;
	queue->Signal(m_fence.Get(), signalvalue);
}

void SyncronizationUnit::WaitFence()

{
#if USESPINLOCKFORFENCEWAIT
	while (m_fence->GetCompletedValue() != m_expectedvalue) {}
#else
	if (m_fence->GetCompletedValue() != m_expectedvalue)
	{

		DXASSERT(m_fence->SetEventOnCompletion(m_expectedvalue, m_event))
			DWORD waitstatus = WaitForSingleObject(m_event, INFINITE);
		assert(waitstatus == WAIT_OBJECT_0);

	}
#endif // USESPINLOCKFORFENCEWAIT

	

}