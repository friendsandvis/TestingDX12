#pragma once
#include"DXUtils.h"



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


