#include"DX12ApplicationManagerBase.h"


DX12ApplicationManagerBase::DX12ApplicationManagerBase()
{
}

DX12ApplicationManagerBase::~DX12ApplicationManagerBase()
{
}

D3D12_VIEWPORT DX12ApplicationManagerBase::GetViewport()
{
	D3D12_VIEWPORT aviewport = {};
	aviewport.TopLeftX = 0;
	aviewport.TopLeftY = 0;
	aviewport.Width = m_swapchain.GetSwapchainWidth();
	aviewport.Height = m_swapchain.GetSwapchainHeight();
	aviewport.MinDepth = 0.0f;
	aviewport.MaxDepth = 1.0f;
	return aviewport;
}
D3D12_RECT DX12ApplicationManagerBase::GetScissorRect()
{
	D3D12_RECT ascissorrect = {};
	ascissorrect.left = 0;
	ascissorrect.top = 0;
	ascissorrect.right = m_swapchain.GetSwapchainWidth();
	ascissorrect.bottom = m_swapchain.GetSwapchainHeight();
	return ascissorrect;
}


void DX12ApplicationManagerBase::Initswapchain(ComPtr<IDXGIFactory2> factory, unsigned width, unsigned height, HWND hwnd)
{
	m_swapchain.Init(factory, width, height, hwnd, m_mainqueue.GetQueue());

	//init rtv heap as well to create backbuffer rtvs
	D3D12_DESCRIPTOR_HEAP_DESC rtvheapdesc = {};
	rtvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvheapdesc.NumDescriptors = BACKBUFFERCOUNT;	//just creating for swapchain rtvs

	m_rtvdescheap.Init(rtvheapdesc, m_creationdevice);

	//create rtvs
	m_swapchain.InitBackBufferRTV(m_creationdevice, m_rtvdescheap);
}

void DX12ApplicationManagerBase::Init(ComPtr< ID3D12Device> creationdevice)
{
	//basic initialization
	InitBase(creationdevice);
	//extra init maybe specialized by specialized classes
	InitExtras();
}

void DX12ApplicationManagerBase::InitBase(ComPtr< ID3D12Device> creationdevice)
{

	m_creationdevice = creationdevice;
	D3D12_COMMAND_QUEUE_DESC mainqueuedesc = {};
	mainqueuedesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	mainqueuedesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	mainqueuedesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	m_mainqueue.Init(mainqueuedesc, m_creationdevice);
	m_syncunitprime.Init(m_creationdevice, 0);

	//commandlist init
	m_primarycmdlist.Init(D3D12_COMMAND_LIST_TYPE_DIRECT, m_creationdevice);
	m_primarycmdlist.SetName(L"primarycmdlist");
	m_uploadcommandlist.Init(D3D12_COMMAND_LIST_TYPE_DIRECT, m_creationdevice);
	m_uploadcommandlist.SetName(L"uploadcmdlist");



}

void DX12ApplicationManagerBase::BasicRender()
{
	std::vector<ID3D12CommandList*> commandliststoexecute;
	commandliststoexecute.push_back(m_uploadcommandlist.GetcmdList());
	commandliststoexecute.push_back(m_primarycmdlist.GetcmdList());
	m_mainqueue.GetQueue()->ExecuteCommandLists(commandliststoexecute.size(), commandliststoexecute.data());

	UINT64 fencevalue = m_syncunitprime.GetCurrentValue();
	fencevalue += 1;
	m_syncunitprime.SignalFence(m_mainqueue.GetQueue(), fencevalue);
	DXASSERT(m_swapchain.GetSwapchain()->Present(0, 0))
		m_syncunitprime.WaitFence();
	m_swapchain.UpdatebackbufferIndex();
	m_uploadcommandlist.Reset(true);
}
	