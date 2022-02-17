#pragma once


#include"DXUtils.h"
#include"Renderable.h"
#include"DX12ApplicationManagerBase.h"


class DX12Manager:public Renderable
{
public:
	DX12Manager();
	~DX12Manager();

	void Init(bool enabledebuglayer = true, DX12ApplicationManagerBase* targetappmanager=nullptr);
	void Render()override;
	inline ComPtr<IDXGIFactory2> GetDXGIFactory() { return m_dxgifactory; }
	inline ComPtr<ID3D12Device> GetDevice() { return m_maindevice; }
private:

	void SeletHardwareAdapter();

	ComPtr<ID3D12Device> m_maindevice;
	ComPtr<IDXGIFactory2> m_dxgifactory;

	std::vector<ComPtr<IDXGIAdapter>> m_alladapters;
	ComPtr<IDXGIAdapter> m_hardwareadapter;
	DX12ApplicationManagerBase* m_appmanager;
};


