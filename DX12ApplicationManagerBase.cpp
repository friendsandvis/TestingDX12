#include"DX12ApplicationManagerBase.h"
#include<chrono>


DX12ApplicationManagerBase::DX12ApplicationManagerBase()
	:m_cmdlistidxinuse(0),
	m_primarycmdlist(m_primarycmdlists[0]),
	m_uploadcommandlist(m_uploadcommandlists[0])
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
	//update the aspect ratio for the main camera
	
	m_maincamera.SetAspectRatio(m_swapchain.GetSwapchainWidth(), m_swapchain.GetSwapchainHeight());
	//create depth buffer & dsv
	{
		DX12ResourceCreationProperties depthbufferprops = {};
		DX12Resource::InitResourceCreationProperties(depthbufferprops);
		depthbufferprops.resdesc.Width = width;
		depthbufferprops.resdesc.Height = height;
		depthbufferprops.resdesc.Format = DXGI_FORMAT_D32_FLOAT;//just a depth buffer for now no stencil
		depthbufferprops.useclearvalue = true;
		depthbufferprops.resinitialstate = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		depthbufferprops.resdesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depthbufferprops.optimizedclearvalue.DepthStencil.Depth = 1.0f;
		depthbufferprops.optimizedclearvalue.DepthStencil.Stencil = 0;
		depthbufferprops.optimizedclearvalue.Format = DXGI_FORMAT_D32_FLOAT;//just a depth buffer for now no stencil
		m_depthbuffer.Init(m_creationdevice, depthbufferprops, ResourceCreationMode::COMMITED);
		m_depthbuffer.SetName(L"DepthBuffer");
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvdesc = {};
		dsvdesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvdesc.Texture2D.MipSlice = 0;
		dsvdesc.Format = DXGI_FORMAT_D32_FLOAT;
		m_depthbuffer.CreateDSV(m_creationdevice, dsvdesc, m_dsvdescheap.GetCPUHandlefromstart());
	}
	//init rtv heap as well to create backbuffer rtvs
	D3D12_DESCRIPTOR_HEAP_DESC rtvheapdesc = {};
	rtvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvheapdesc.NumDescriptors = BACKBUFFERCOUNT;	//just creating for swapchain rtvs

	m_rtvdescheap.Init(rtvheapdesc, m_creationdevice);

	//create rtvs
	m_swapchain.InitBackBufferRTV(m_creationdevice, m_rtvdescheap);
}

void DX12ApplicationManagerBase::Init(ComPtr< ID3D12Device> creationdevice, ComPtr<IDXGIFactory2> factory, unsigned swapchainwidth, unsigned swapchainheight, HWND hwnd)
{
	//basic initialization first
	InitBase(creationdevice);
	Initswapchain(factory, swapchainwidth, swapchainheight, hwnd);
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
	for (unsigned i = 0; i < NUMCOMMANDLISTSTOCK; i++)
	{
		m_primarycmdlists[i].Init(D3D12_COMMAND_LIST_TYPE_DIRECT, m_creationdevice);
		m_primarycmdlists[i].SetName(L"primarycmd");
		
		
		m_uploadcommandlists[i].Init(D3D12_COMMAND_LIST_TYPE_DIRECT, m_creationdevice);
		m_uploadcommandlists[i].SetName(L"uploadcmd");
		m_uploadcommandlist = m_uploadcommandlists[0];
		
	}
	m_primarycmdlist = m_primarycmdlists[0];
	m_uploadcommandlist = m_uploadcommandlists[0];
	m_cmdlistidxinuse = 0;

	//dsv heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvheapdesc = {};
	dsvheapdesc.NodeMask = 0;
	dsvheapdesc.NumDescriptors = 1;
	dsvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	m_dsvdescheap.Init(dsvheapdesc, m_creationdevice);
	


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
	m_cmdlistidxinuse = (m_cmdlistidxinuse + 1) % NUMCOMMANDLISTSTOCK;
	m_primarycmdlist = m_primarycmdlists[m_cmdlistidxinuse];
	m_uploadcommandlist = m_uploadcommandlists[m_cmdlistidxinuse];
}
void DX12ApplicationManagerBase::ClearBackBuffer(unsigned backbufferindex, DX12Commandlist& cmdlisttouse,float clearvalue[4])
{
	
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(backbufferindex);
	D3D12_RESOURCE_BARRIER barrier=m_swapchain.TransitionBackBuffer(backbufferindex, D3D12_RESOURCE_STATE_RENDER_TARGET);
	if(DXUtils::IsBarrierSafeToExecute(barrier));
	{
		cmdlisttouse->ResourceBarrier(1, &barrier);
	}
	cmdlisttouse->ClearRenderTargetView(rtvhandle, clearvalue, 0, nullptr);
	barrier = m_swapchain.TransitionBackBuffer(backbufferindex, D3D12_RESOURCE_STATE_COMMON);
	cmdlisttouse->ResourceBarrier(1, &barrier);
}
	
