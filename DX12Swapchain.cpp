#include"DX12Swapchain.h"

DX12Swapchain::DX12Swapchain()
{
}

DX12Swapchain::~DX12Swapchain()
{
}

UINT DX12Swapchain::GetCurrentbackbufferIndex()
{
	return m_currentbackbufferindex;
}
UINT DX12Swapchain::UpdateandGetCurrentbackbufferIndex()
{
	m_currentbackbufferindex = m_swapchainv4->GetCurrentBackBufferIndex();
	return m_currentbackbufferindex;
}

void DX12Swapchain::UpdatebackbufferIndex()
{
	m_currentbackbufferindex = m_swapchainv4->GetCurrentBackBufferIndex();
}

void DX12Swapchain::InitBackBufferRTV(ComPtr< ID3D12Device> creationdevice, DX12DESCHEAP& rtvdescheap)
{
	
	D3D12_CPU_DESCRIPTOR_HANDLE start = rtvdescheap.GetCPUHandlefromstart();

	for (size_t i = 0; i < BACKBUFFERCOUNT; i++)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE target = rtvdescheap.GetCPUHandleOffseted(i);

		creationdevice->CreateRenderTargetView(m_backbuffers[i].Get(), nullptr, target);
	
	}

}

void DX12Swapchain::Init(ComPtr<IDXGIFactory2> factory, unsigned width, unsigned height, HWND hwnd, ComPtr<ID3D12CommandQueue> creationqueue)
{
	
	DXGI_SWAP_CHAIN_DESC1 swapchaindesc = {};
	swapchaindesc.BufferCount = BACKBUFFERCOUNT;
	swapchaindesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapchaindesc.Width = width;
	swapchaindesc.Height = height;
	swapchaindesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.Stereo = FALSE;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.SampleDesc.Quality = 0;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	DXASSERT(factory->CreateSwapChainForHwnd(creationqueue.Get(), hwnd, &swapchaindesc, nullptr, nullptr, m_swapchain.GetAddressOf()))
		DXASSERT(m_swapchain.As< IDXGISwapChain4>(&m_swapchainv4))
		m_currentbackbufferindex = m_swapchainv4->GetCurrentBackBufferIndex();
		RetiveBackBuffers();
}

void DX12Swapchain::RetiveBackBuffers()
{
	
	for (size_t i = 0; i < BACKBUFFERCOUNT; i++)
	{
		DXASSERT(m_swapchain->GetBuffer(i, IID_PPV_ARGS(m_backbuffers[i].GetAddressOf())))
			
	}
}