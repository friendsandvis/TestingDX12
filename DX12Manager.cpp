#include"DX12Manager.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//if we want to use agility sdk downloaded from nuget we can uncomment this and set sdk version properly in the variable.
//#define USEAGILITYSDK

//exports to use agility sdk...

//version of agility sdk nuget used(update if using another version.
#ifdef USEAGILITYSDK

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 600; }

extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }
#endif // USEAGILITYSDK


DX12Manager::DX12Manager()
{
	DXASSERT(CreateDXGIFactory2(0, IID_PPV_ARGS(m_dxgifactory.GetAddressOf())))

		//retrive all adapters

		UINT adapterindex = 0;
	ComPtr<IDXGIAdapter> anadapter;
		while (m_dxgifactory->EnumAdapters(adapterindex, anadapter.GetAddressOf()) == S_OK)
		{
			m_alladapters.push_back(anadapter);
			adapterindex++;
		}
		//choose the hardware adapter here
		SeletHardwareAdapter();
}

DX12Manager::~DX12Manager()
{
}

void DX12Manager::Init(unsigned swapchainwidth, unsigned swapchainheight, HWND hwnd, bool enabledebuglayer, DX12ApplicationManagerBase* targetappmanager)
{
	m_appmanager = targetappmanager;
	

	//debug layer if requested
	if (enabledebuglayer)
	{
		ComPtr<ID3D12Debug5> debuginterterface;
		DXASSERT(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)debuginterterface.GetAddressOf()))
			debuginterterface->EnableDebugLayer();
		//debuginterterface->SetEnableGPUBasedValidation(true);
		//debuginterterface->SetEnableAutoName(true);

		
		

	}
	//create device
	DXASSERT(D3D12CreateDevice(m_hardwareadapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_maindevice.GetAddressOf())))
		m_maindevice->SetName(L"MainDevice");
	
	
		//initialize app manager
		if (targetappmanager)
		{
			//first swap chain then app init.
			//m_appmanager->Initswapchain(m_dxgifactory, swapchainwidth, swapchainheight, hwnd);
			m_appmanager->Init(m_maindevice,m_dxgifactory,swapchainwidth,swapchainheight,hwnd);
		}
}

void DX12Manager::SeletHardwareAdapter()
{
	unsigned choosenadapter = 0;
	for (; choosenadapter < m_alladapters.size(); choosenadapter++)
	{
		DXGI_ADAPTER_DESC adapterdesc;
		m_alladapters[choosenadapter]->GetDesc(&adapterdesc);

		// break upon nvidia adapter
		if (adapterdesc.VendorId == HARDWAREVENDORID)
		{
			m_hardwareadapter = m_alladapters[choosenadapter];
			break;
		}



	}
}

void DX12Manager::Render()
{
	if (m_appmanager)
	{
		m_appmanager->PreRenderUpdate();
		m_appmanager->Render();
		m_appmanager->PostRenderUpdate();
	}
}