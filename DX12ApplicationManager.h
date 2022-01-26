#pragma once
#include"DX12CommandQueue.h"
#include"DX12Swapchain.h"
#include"DX12DESCHEAP.h"
#include"DX12PSO.h"

//maybe make it an abstract class to have application based psos
class DX12ApplicationManager
{
public:
	DX12ApplicationManager();
	~DX12ApplicationManager();

	void Init(ComPtr< ID3D12Device> creationdevice);
	void InitBasicPSO();
	void Initswapchain(ComPtr<IDXGIFactory2> factory, unsigned width, unsigned height, HWND hwnd);
	
	void Render();
	
private:
	ComPtr< ID3D12Device> m_creationdevice;
	DX12CommandQueue m_mainqueue;
	DX12Swapchain m_swapchain;
	DX12DESCHEAP m_rtvdescheap;
	DX12PSO m_basicpso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob,m_rootsignatureerrors;
	ComPtr<ID3D12RootSignature> m_emptyrootsignature;
};


