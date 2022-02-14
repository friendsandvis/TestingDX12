#include"DX12Manager.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


//exports to use agility sdk...

//version of agility sdk nuget used(update if using another version.
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 600; }

extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }



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

void DX12Manager::Init(bool enabledebuglayer, DX12ApplicationManager* targetappmanager)
{
	//create device

	//debug layer if requested
	if (enabledebuglayer)
	{
		ComPtr<ID3D12Debug> debuginterterface;
		DXASSERT(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)debuginterterface.GetAddressOf()))
			debuginterterface->EnableDebugLayer();

		m_appmanager = targetappmanager;
		

	}
	
	DXASSERT(D3D12CreateDevice(m_hardwareadapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_maindevice.GetAddressOf())))
		m_maindevice->SetName(L"MainDevice");
	
		//initialize app manager
		if (targetappmanager)
		{
			
			m_appmanager->Init(m_maindevice);
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
		m_appmanager->Render();
	}
}