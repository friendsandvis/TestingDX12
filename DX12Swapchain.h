#pragma once
#include"DXUtils.h"
#include"DX12DESCHEAP.h"
#include<dxgi1_5.h>

class DX12Swapchain
{
public:
	DX12Swapchain();
	~DX12Swapchain();

	UINT GetCurrentbackbufferIndex();
	UINT UpdateandGetCurrentbackbufferIndex();
	void InitBackBufferRTV(ComPtr< ID3D12Device> creationdevice, DX12DESCHEAP& rtvdescheap );

	void Init(ComPtr<IDXGIFactory2> factory, unsigned width, unsigned height, HWND hwnd, ComPtr<ID3D12CommandQueue> creationqueue);
private:
	
	void RetiveBackBuffers();
	ComPtr<IDXGISwapChain1> m_swapchain;
	ComPtr< IDXGISwapChain4> m_swapchainv4;
	ComPtr<ID3D12Resource> m_backbuffers[BACKBUFFERCOUNT];
	UINT m_currentbackbufferindex;
};


