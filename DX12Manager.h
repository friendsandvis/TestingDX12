#pragma once

#include<d3d12.h>
#include<dxgi1_3.h>
#include"DXUtils.h"


class DX12Manager
{
public:
	DX12Manager();
	~DX12Manager();

	void Init(bool enabledebuglayer = true);

private:
	ComPtr<ID3D12Device> m_maindevice;
	ComPtr<IDXGIFactory2> m_dxgifactory;
};


