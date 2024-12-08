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

		creationdevice->CreateRenderTargetView(m_backbuffers[i].resource.Get(), nullptr, target);
	
	}

}

D3D12_RESOURCE_BARRIER DX12Swapchain::TransitionBackBuffer(UINT backbufferindex, D3D12_RESOURCE_STATES targetstate)
{
	D3D12_RESOURCE_BARRIER transitionbarrier = {};
	transitionbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	transitionbarrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
	transitionbarrier.Transition.pResource = m_backbuffers[backbufferindex].resource.Get();
	transitionbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	transitionbarrier.Transition.StateAfter = targetstate;
	transitionbarrier.Transition.StateBefore = m_backbuffers[backbufferindex].currentstate;
	m_backbuffers[backbufferindex].currentstate = targetstate;
	return transitionbarrier;
}

void DX12Swapchain::Init(ComPtr<IDXGIFactory2> factory, unsigned width, unsigned height, HWND hwnd, ComPtr<ID3D12CommandQueue> creationqueue)
{
	DXGI_SWAP_CHAIN_DESC1 swapchaindesc = {};
	swapchaindesc.BufferCount = BACKBUFFERCOUNT;
	swapchaindesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	//using width & height selected by create swapchain command helps fix imgui bug where imgui has y offseted a little;hence we pass 0 as dimensions and retrive it later to be saved as members
	swapchaindesc.Width = 0;
		swapchaindesc.Height = 0;
	swapchaindesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapchaindesc.Stereo = FALSE;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	////lets use scaling as we are not deciding the swapchain size anymore(seen in imgui sample)
	swapchaindesc.Scaling = DXGI_SCALING_STRETCH;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.SampleDesc.Quality = 0;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	DXASSERT(factory->CreateSwapChainForHwnd(creationqueue.Get(), hwnd, &swapchaindesc, nullptr, nullptr, m_swapchain.GetAddressOf()))
		DXASSERT(m_swapchain.As< IDXGISwapChain4>(&m_swapchainv4))
		m_currentbackbufferindex = m_swapchainv4->GetCurrentBackBufferIndex();
	DXGI_SWAP_CHAIN_DESC1 swapchaindescretrived = {};
	m_swapchain->GetDesc1(&swapchaindescretrived);
	//we are retriving width & heigght of swapchain selected by swapchsain creation from hwnd usually smaller than the window size
	m_width = swapchaindescretrived.Width;
	m_height = swapchaindescretrived.Height;

		RetiveBackBuffers();
}

void DX12Swapchain::RetiveBackBuffers()
{
	
	for (size_t i = 0; i < BACKBUFFERCOUNT; i++)
	{
		DX12ResourceWrapper tmpresourcewrapper = {};
		tmpresourcewrapper.currentstate = D3D12_RESOURCE_STATE_PRESENT;
		m_backbuffers[i] = tmpresourcewrapper;
		DXASSERT(m_swapchain->GetBuffer(i, IID_PPV_ARGS(m_backbuffers[i].resource.GetAddressOf())))
			m_backbuffers[i].resource->SetName(L"swapchainbackbuffer");
			m_backbuffers[i].currentstate = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
			
	}
}