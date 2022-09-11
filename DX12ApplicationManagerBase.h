#pragma once
#include"DXUtils.h"
#include"DX12Swapchain.h"
#include"DX12CommandList.h"
#include"DX12DESCHEAP.h"
#include"DX12CommandQueue.h"
#include"DX12Resource.h"
#include"WindowProcHook.h"
#include"DXCamera.h"
#define NUMCOMMANDLISTSTOCK 2

// interface for all dx12 applicationmanagers.Could help in creaing  speciialized dx12 applications
class DX12ApplicationManagerBase:public WindowProcHook
{
public:
	void Init(ComPtr< ID3D12Device> creationdevice, ComPtr<IDXGIFactory2> factory, unsigned swapchainwidth, unsigned swapchainheight, HWND hwnd);
	//basic necessary initialization of 
	void InitBase(ComPtr< ID3D12Device> creationdevice);
	void Initswapchain(ComPtr<IDXGIFactory2> factory, unsigned width, unsigned height, HWND hwnd);
	virtual void Render() = 0;
	virtual void PreRenderUpdate() {}
	virtual void PostRenderUpdate() {}
	//called inside init function san be overriden by child class to init other app specific members at app init time
	

	//execute commandlists in a fixed order(upload then primary) & present
	//if the application does not wish to modify the application's default execution model then use this else write a custom render function with presentation.
	//Warning: updates swapchain backbuffer index
	void BasicRender();
	DX12ApplicationManagerBase();
	virtual ~DX12ApplicationManagerBase();

protected:
	//a camera object representing the main camera for a basic application
	DXCamera m_maincamera;
	 virtual void InitExtras() {}
	

	ComPtr< ID3D12Device> m_creationdevice;
	DX12CommandQueue m_mainqueue;

	// base app has 2 commandlists(1 for general render other for upload before render
	DX12Commandlist& m_primarycmdlist, m_uploadcommandlist;
	DX12Commandlist m_primarycmdlists[NUMCOMMANDLISTSTOCK], m_uploadcommandlists[NUMCOMMANDLISTSTOCK];
	unsigned m_cmdlistidxinuse;

	SyncronizationUnit m_syncunitprime;
	DX12Swapchain m_swapchain;

	//returns the default viewport based on the current swapchain properties
	D3D12_VIEWPORT GetViewport();
	//returns the default scissor rect based on the current swapchain properties
	D3D12_RECT GetScissorRect();

	//this one is just for swapchain buffers
	DX12DESCHEAP m_rtvdescheap;
	DX12DESCHEAP m_dsvdescheap;
	DX12Resource m_depthbuffer;
};

