#pragma once
#include"DXUtils.h"


class SyncronizationUnit
{
public:
	void Init(ComPtr<ID3D12Device> creationdevice, UINT64 initvalue);
	SyncronizationUnit();
	~SyncronizationUnit();
	UINT64 GetCurrentValue() { return m_currentvalue; }
	void SignalFence(ComPtr<ID3D12CommandQueue> queue,UINT64 signalvalue);//signals fence from gpu
	void WaitFence();

private:
	ComPtr<ID3D12Fence> m_fence;
	HANDLE m_event;
	UINT64 m_expectedvalue, m_currentvalue;
};



class DX12CommandQueue
{
public:
	DX12CommandQueue();
	~DX12CommandQueue();

	void Init(D3D12_COMMAND_QUEUE_DESC queuedesc,ComPtr<ID3D12Device> creationdevice);
	inline ComPtr<ID3D12CommandQueue> GetQueue() { return m_queue; }

private:
	D3D12_COMMAND_QUEUE_DESC m_desc;
	ComPtr<ID3D12CommandQueue> m_queue;
	

};


