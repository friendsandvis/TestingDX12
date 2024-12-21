#pragma once
#include"DXUtils.h"
#include"DX12Swapchain.h"
#include"DX12CommandList.h"
#include"DX12DESCHEAP.h"
#include"DX12CommandQueue.h"
#include"DX12Resource.h"
#include"WindowProcHook.h"
#include"DXCamera.h"
//imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include<imgui.h>
#include<backends/imgui_impl_dx12.h>
#include<backends/imgui_impl_win32.h>

#define NUMCOMMANDLISTSTOCK BACKBUFFERCOUNT

// interface for all dx12 applicationmanagers.Could help in creaing  speciialized dx12 applications
class DX12ApplicationManagerBase:public WindowProcHook
{
public:
	void Init(ComPtr< ID3D12Device> creationdevice, ComPtr<IDXGIFactory2> factory, unsigned swapchainwidth, unsigned swapchainheight, HWND hwnd);
	//basic necessary initialization of 
	void InitBase(ComPtr< ID3D12Device> creationdevice);
	void Initswapchain(ComPtr<IDXGIFactory2> factory, unsigned width, unsigned height, HWND hwnd);
	virtual void Render() = 0;
	virtual void PreRenderUpdate();
	virtual void PostRenderUpdate() {}
	virtual void Destroy();
	virtual void IMGUIRenderAdditional() {}
	inline bool IsIMGUIAllowed()
	{ return m_imguiAllowed; }
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
	 void ClearBackBuffer(unsigned backbufferindex, DX12Commandlist& cmdlisttouse, float clearvalue[4]);
	 void InitIMGUI(ComPtr< ID3D12Device> creationdevice, HWND hwnd);
	 void IMGUIPrerender();
	

	ComPtr< ID3D12Device> m_creationdevice;
	DX12CommandQueue m_mainqueue;

	// base app has 2 commandlists(1 for general render other for upload before render
	DX12Commandlist m_primarycmdlist, m_uploadcommandlist, m_prepresentcommandlist;
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
	//imgui related
	DX12DESCHEAP m_imguisrvdescheap;

	//frame buffering stuff
	UINT64 m_lastSignaledFenceValue;
	UINT m_frameIdx;
	UINT64 m_frameFenceValue[BACKBUFFERCOUNT] = {0};
	//to allow upload commandlist to execute only once
	bool m_executedUploadcmdlist;
	bool m_imguiAllowed;
};

